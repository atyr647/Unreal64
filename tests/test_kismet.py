import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools", "ue64"))
import codegen  # noqa: E402
import kismet  # noqa: E402

# BP_MinionSpawner.ReceiveBeginPlay as stored in the package (UE 4.2x, 4-byte object refs); in-memory size 22.
BEGIN_PLAY = bytes.fromhex("5e5a5e46070000001d9800000016" "5a040b53")


def decode(script, names=(), resolve=lambda i: {7: "ExecuteUbergraph_X"}.get(i, str(i))):
    d = kismet.Decoder(script, list(names), resolve, 517, 0)
    st = d.run()
    return d, st


class KismetTests(unittest.TestCase):
    def test_begin_play_decodes_with_matching_memory_size(self):
        d, st = decode(BEGIN_PLAY)
        self.assertEqual(d.mem, 22)  # pointers are 8 bytes in memory, 4 on disk
        self.assertEqual([n.op for n in st], ["Tracepoint", "WireTracepoint", "Tracepoint", "LocalFinalFunction",
                                              "WireTracepoint", "Return", "EndOfScript"])
        call = next(n for n in st if n.op == "LocalFinalFunction")
        self.assertEqual(call.args[0], "ExecuteUbergraph_X")
        self.assertEqual(call.args[1][0].args, [152])  # event graph entry point

    def test_truncated_or_unterminated_script_is_rejected(self):
        with self.assertRaises(kismet.KismetError):
            decode(BEGIN_PLAY[:-1])
        with self.assertRaises(kismet.KismetError):
            decode(bytes.fromhex("5e5a") + b"\x04\x0b")  # no EndOfScript

    def test_jump_targets_are_validated(self):
        # Jump to an offset that is not a token start must be reported.
        script = bytes.fromhex("06" "63000000" "53")
        d, st = decode(script)
        self.assertEqual(len(d.check_jumps(st)), 1)

    def test_codegen_emits_goto_and_call(self):
        _, st = decode(BEGIN_PLAY)
        src, g = codegen.gen_function("BP_X", "ReceiveBeginPlay", st)
        self.assertIn('U64_CALL(self, "ExecuteUbergraph_X", u64_int(152))', src)
        self.assertIn("u64_ret:;", src)
        self.assertEqual(g.unsupported, [])


if __name__ == "__main__":
    unittest.main()
