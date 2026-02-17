MyInfo = provider(fields = {
    "val": "string value",
    "out": "output File",
})

def _glsl_source_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".out")
    ctx.actions.write(out, "abc")
    return [MyInfo(val = "some value", out = out)]

glsl_source = rule(
    implementation = _glsl_source_impl,
    attrs = {
        "directory": attr.string(mandatory = True),
        "stages": attr.string_list(mandatory = True),
    },
)
