# Unreal64 — plan

## The constraint that decides everything

An N64 has a 93.75 MHz VR4300, 4 MB of RAM (8 MB with the Expansion Pak) and a fixed-function RDP.
Unreal Engine 5 needs gigabytes of RAM and a modern GPU. That is a gap of roughly **1000x in memory**
before counting compute, and the RDP has no programmable shaders, let alone ray tracing.
No port, however clever, runs Lumen, Nanite, hardware RT or path tracing *on* an N64, and none can
run a modern UE project unmodified on one.

So the goal "unmodified latest UE project, with all the bells and whistles, on an N64" has exactly
one honest reading, and it is the one this repo implements:

> **The unmodified Unreal project runs at full fidelity on a host GPU. The N64 is the display and the
> controller.** A bridge captures the game's output, converts it to what the N64 can show, and sends
> it over the flashcart's USB link. The N64 sends its controller state back.

What that does and does not give you:

| Wanted | Result |
|---|---|
| Unmodified project, any UE version | **Yes.** Nothing is injected into the engine or project. Capture is a window grab; input is a virtual gamepad. |
| Lumen, Nanite, HW ray tracing, path tracing | **Yes, on the host.** They run exactly as the project configures them. They are *not* running on the N64. |
| Those effects visible on the N64 screen | **Heavily degraded.** The picture is downscaled to ~320x240, 15/16-bit colour, dithered. GI and reflections survive as lighting; fine detail does not. |
| Playable | **Depends on link throughput and latency**, which is unmeasured. See risks. |
| Running on N64 with no PC | **No.** Not possible. |

## Architecture

```
 Unreal game (unmodified)        host/unreal64 bridge                    N64 (libdragon ROM)
 ┌──────────────────────┐   ┌──────────────────────────────┐   USB   ┌────────────────────────┐
 │ Lumen/Nanite/RT/PT   │   │ capture (ffmpeg window grab)  │ ──────► │ receive, rebuild frame │
 │ renders at full res  │──►│ scale → quantise → delta/comp │  frames │ show via VI            │
 │                      │◄──│ virtual gamepad (uinput/ViGEm)│ ◄────── │ read pad, send state   │
 └──────────────────────┘   └──────────────────────────────┘  input  └────────────────────────┘
```

Why capture rather than an engine plugin: a plugin is a change to the project/engine and breaks the
"unmodified, latest version" requirement. A window grab works with any version and any project.

## Bandwidth is the central problem

320x240 at 16 bpp is 153,600 B/frame, 4.6 MB/s at 30 fps raw. The link's real throughput is **unknown**
until measured on hardware, and libdragon's generic USB path (`usb_read`) pulls 512-byte polled
blocks, which is likely slow. The host sim (`python -m unreal64 selftest`, 2 MB/s assumed) shows:
a mostly-static scene converges losslessly, but fast full-screen motion leaves ~25% of pixels stale.
Tile deltas alone are not enough for gameplay; compression is on the critical path.

Options, in the order I'd try them:
1. **Measure first** (Phase 2). Everything else is sized from that number.
2. **8-bit palettised frames** (CI8 + per-frame optimised palette, error diffusion): halves bytes, usually
   looks better than dithered 5551 at the same budget.
3. **Lower resolution** (256x192 / 160x120) stretched by the VI scaler: cheap, large byte savings.
4. **MPEG-1 decoded on the RSP.** Best quality per byte; libdragon has had RSP video decoding, but I have
   not confirmed it exists in the current release. Verify before committing to it.
5. **Cart-specific bulk USB read** replacing libdragon's generic debug path, if (1) shows it is the bottleneck.

## Phases

| # | Phase | State |
|---|---|---|
| 0 | Wire protocol, host quantiser + delta encoder with byte budget, simulated receiver, tests, N64 client source | **Done.** Host side tested (8 tests). N64 source passes a syntax/type check against libdragon headers only. |
| 1 | Build the MIPS toolchain + libdragon, build `unreal64.z64`, boot it in an emulator with a canned frame | **Not done.** Toolchain build was not run in this session. |
| 2 | Real hardware: pick cart (SummerCart64 / EverDrive / 64drive), write the host transport, **measure throughput and latency** | Not started. Needs hardware. |
| 3 | Compression (options above), chosen from Phase 2 data | Not started |
| 4 | Input backends: Linux uinput, Windows ViGEm virtual Xbox pad; stick curve, deadzone, button map profiles | Interface only (`inputmap.py`) |
| 5 | Audio: capture, downmix, 16-bit ~22–32 kHz to the N64 audio interface | Not started |
| 6 | Quality pass: tone-map/gamma for 5551, temporal dither, HUD-aware tile priority, adaptive budget | Not started |
| 7 | Optional: remote UE host (cloud GPU) with a small bridge box next to the N64 | Not started |

## Keeping the project unmodified

Everything below is outside the project and uses no engine changes:
- Launch args, e.g. `-windowed -ResX=1280 -ResY=720` (small window = cheap capture; the N64 downscales anyway).
- Whatever RT/Lumen/Nanite/path-tracing settings the project already ships with. Hardware RT needs DX12 and an RT-capable GPU on the host; I have not touched this.

## Risks / open questions

1. **Link throughput and latency**: unmeasured; could make the whole thing unplayable. Phase 2 answers it.
2. **Capture latency** (ffmpeg grab → scale → encode) adds to input-to-photon delay; may need a faster grabber (DXGI/NvFBC).
3. **libdragon API drift**: only checked against the current master headers by syntax check.
4. **Hardware needed to go further**: which flashcart do you own, and is the host Windows or Linux?
