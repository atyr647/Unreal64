# Unreal64

Show an **unmodified Unreal Engine project** on a real N64 — by running Unreal on a host GPU at full
quality (Lumen, Nanite, hardware RT, path tracing, whatever the project uses) and streaming the picture
to the N64 over a flashcart's USB link, with the N64 controller driving the game.

The engine cannot run *on* an N64 (4–8 MB RAM, no programmable shading). Read [docs/PLAN.md](docs/PLAN.md)
first: it says exactly what this does and does not deliver, and what is still unproven.

## Status

- `host/unreal64/`: protocol, RGBA5551 dithered quantiser, delta/tile encoder with byte budget, reference receiver, ffmpeg capture. Tested.
- `n64/`: libdragon client. Syntax/type-checked against libdragon headers only; **never built into a ROM or run on hardware.**
- Not done: toolchain build, hardware transport, throughput measurement, compression, audio, real input injectors.

## Try the host side (no hardware)

```
python3 -m unittest discover -s tests
cd host && python3 -m unreal64 selftest --link-mbps 2 --png out.png
cd host && python3 -m unreal64 stream --lavfi "testsrc2=size=1920x1080:rate=60" --png out.png   # needs ffmpeg
```
Requires Python 3.10+, numpy, Pillow (PNG output), ffmpeg (capture).
