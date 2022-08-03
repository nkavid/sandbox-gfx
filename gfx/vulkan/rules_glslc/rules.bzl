def _glsl_shader(ctx):
    spirv_name = ctx.file.shader.basename.removesuffix(".glsl") + ".spv"

    spirv_output = ctx.actions.declare_file(spirv_name, sibling = ctx.file.shader)

    args = ctx.actions.args()

    if ctx.file.shader.basename.endswith(".vert.glsl"):
        args.add("-fshader-stage=vertex", ctx.file.shader)

    if ctx.file.shader.basename.endswith(".frag.glsl"):
        args.add("-fshader-stage=fragment", ctx.file.shader)

    args.add("-Werror")
    args.add("-o", spirv_output)

    ctx.actions.run(
        outputs = [spirv_output],
        inputs = [ctx.file.shader],
        arguments = [args],
        executable = "glslc",
        progress_message = "compiling GLSL",
        mnemonic = "GLSLC",
    )

    runfiles = ctx.runfiles(
        files = [spirv_output],
    )

    default_files = depset(direct = [spirv_output])

    return [
        DefaultInfo(files = default_files, runfiles = runfiles),
    ]

glsl_shader = rule(
    implementation = _glsl_shader,
    attrs = {
        "shader": attr.label(allow_single_file = [
            ".vert.glsl",
            ".frag.glsl",
            ".comp.glsl",
        ]),
    },
    provides = [DefaultInfo],
)
