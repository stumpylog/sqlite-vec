fn main() {
    println!("cargo:rerun-if-env-changed=OMIT_SIMD_BENCH");
    let target_arch = std::env::var("CARGO_CFG_TARGET_ARCH").unwrap_or_default();

    let mut build = cc::Build::new();
    build
        .file("../../sqlite-vec.c")
        .include("../../vendor")
        .include("../../");

    if target_arch == "x86_64" {
        let omit_simd = std::env::var("OMIT_SIMD_BENCH").map(|v| v == "1").unwrap_or(false);
        if !omit_simd {
            build.define("SQLITE_VEC_ENABLE_AVX", None);
        }
        // Force SSE2 baseline so the auto-vectorizer cannot use AVX on the scalar
        // fallback paths. The explicit __attribute__((target(...))) kernels still
        // compile with their own target, so the benchmark measures the real gain
        // of explicit SIMD dispatch vs. a genuine scalar baseline.
        // OMIT_SIMD_BENCH=1 skips the SQLITE_VEC_ENABLE_AVX define entirely,
        // producing a pure-scalar build to compare against.
        build.flag("-march=x86-64");
    }

    build.compile("sqlite_vec0");
}
