fn main() {
    let target_arch = std::env::var("CARGO_CFG_TARGET_ARCH").unwrap_or_default();

    let mut build = cc::Build::new();
    build
        .file("../../sqlite-vec.c")
        .include("../../vendor")
        .include("../../");

    if target_arch == "x86_64" {
        build.define("SQLITE_VEC_ENABLE_AVX", None);
    }

    build.compile("sqlite_vec0");
}
