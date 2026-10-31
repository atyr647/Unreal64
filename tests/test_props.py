import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools", "ue64"))
import kismet  # noqa: E402
import props  # noqa: E402
import uasset  # noqa: E402

NAMES = ["None", "Value", "IntProperty", "Items", "ArrayProperty", "StructProperty", "Inner", "Gen", "Health", "Prop"]
N = {n: i for i, n in enumerate(NAMES)}


class FakePackage:
    names, ue4, ue5, imports, exports = NAMES, 517, 0, [], []


def fn(name): return struct.pack("<ii", N[name], 0)
def tag(name, typ, size, extra=b""): return fn(name) + fn(typ) + struct.pack("<ii", size, 0) + extra + b"\x00"   # property-guid flag


class PropsTests(unittest.TestCase):
    def read(self, blob):
        r = uasset.R(blob + b"\0" * 16)
        return props.read_stream(r, props.Ctx(FakePackage, blob), len(blob)), r

    def test_scalar_and_terminator(self):
        blob = tag("Value", "IntProperty", 4) + struct.pack("<i", 42) + fn("None")
        out, r = self.read(blob)
        self.assertEqual(out, {"Value": 42})
        self.assertEqual(r.p, len(blob))

    def test_empty_struct_array_consumes_its_inner_header(self):
        # An empty TArray<struct> still carries a (name, type, size, ...) inner tag; skipping it desyncs the stream.
        inner = fn("Inner") + fn("StructProperty") + struct.pack("<ii", 0, 0) + fn("Gen") + b"\0" * 16 + b"\0"
        body = struct.pack("<i", 0) + inner
        blob = tag("Items", "ArrayProperty", len(body), fn("StructProperty")) + body + tag("Value", "IntProperty", 4) + struct.pack("<i", 5) + fn("None")
        out, _ = self.read(blob)
        self.assertEqual(out["Items"], ("array", []))
        self.assertEqual(out["Value"], 5)            # the property after the empty array is still read correctly

    def test_value_that_disagrees_with_declared_size_is_kept_raw(self):
        blob = tag("Value", "IntProperty", 8) + struct.pack("<ii", 1, 2) + fn("None")
        out, _ = self.read(blob)
        self.assertEqual(out["Value"][0], "raw")


class FieldPathTests(unittest.TestCase):
    def test_innermost_name_is_the_property(self):
        # UE 4.25-style field path in a script: count, then names innermost first: [property, class, function, package].
        names = ["Health", "MyClass", "MyFunc", "/Game/Pkg"]
        script = bytes([0x00]) + struct.pack("<i", 4) + b"".join(struct.pack("<ii", i, 0) for i in range(4)) + bytes([0x53])
        d = kismet.Decoder(script, names, str, 518, 0, field_path=True)
        st = d.run()
        self.assertEqual(st[0].op, "LocalVariable")
        self.assertEqual(st[0].args[0], "Health")      # previously returned the outermost name ("/Game/Pkg")


class ParamOrderTests(unittest.TestCase):
    def test_event_stub_pin_order_beats_export_order(self):
        import codegen
        # Export order is (A, B, C), but the event stub copies them to the frame as C, A, B: that is the declaration order.
        N = kismet.Node
        def stub(var): return N(0, 0, "LetValueOnPersistentFrame", ["K2Node_X_" + var, N(0, 0, "LocalVariable", [var])])
        out = codegen.order_params([("A", False), ("B", False), ("C", True)], [stub("C"), stub("A"), stub("B")])
        self.assertEqual([n for n, _ in out], ["C", "A", "B"])
        self.assertTrue(dict(out)["C"])                          # the out flag follows the parameter

    def test_parameters_without_stub_keep_export_order(self):
        import codegen
        out = codegen.order_params([("A", False), ("B", False)], [])
        self.assertEqual([n for n, _ in out], ["A", "B"])


if __name__ == "__main__":
    unittest.main()
