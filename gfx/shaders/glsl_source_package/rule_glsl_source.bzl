MyInfo = provider(fields = {
    "val": "string value",
    "out": "output File",
})

def _glsl_source_impl(ctx):
    print(ctx.label.name)
    print(ctx.attr.directory)
    print(ctx.attr.stages)
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
