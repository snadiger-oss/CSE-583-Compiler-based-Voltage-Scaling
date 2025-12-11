#!/usr/bin/env python3
import re
import os
import argparse
import subprocess

FUNC_TYPE_RE = re.compile(r"\.type\s+([A-Za-z0-9_$.]+),@function")
FUNC_END_RE  = re.compile(r"\.Lfunc_end[0-9]+")


def split_functions(asm_path):
    with open(asm_path, "r") as f:
        lines = f.readlines()

    funcs = {}
    current = None
    buf = []

    for ln in lines:
        m = FUNC_TYPE_RE.search(ln)
        if m:
            if current and buf:
                funcs[current] = buf
            current = m.group(1)
            buf = [ln]
            continue

        if current:
            buf.append(ln)
            if FUNC_END_RE.search(ln):
                funcs[current] = buf
                current = None
                buf = []

    if current and buf:
        funcs[current] = buf

    return funcs


def clean_asm(lines):
    bad = (
        ".cfi_",
        ".loc",
        ".file",
        ".section\t.debug",
        ".ident",
        ".cv_",
    )
    cleaned = []
    for ln in lines:
        if any(ln.strip().startswith(x) for x in bad):
            continue
        cleaned.append(ln)
    return cleaned


def write_funcs(funcs, outdir):
    os.makedirs(outdir, exist_ok=True)
    for name, lines in funcs.items():
        cleaned = clean_asm(lines)
        fpath = os.path.join(outdir, f"{name}.s")
        with open(fpath, "w") as f:
            if not any(".text" in l for l in cleaned):
                f.write("\t.text\n")
            f.writelines(cleaned)


def run_mca(name, sfile, outdir):
    outfile = os.path.join(outdir, f"{name}_mca.json")

    cmd = [
        "llvm-mca",
        "--bottleneck-analysis",
        "--resource-pressure",
        "--json",
        "--skip-unsupported-instructions=parse-failure",
        "-mcpu=skylake",
        sfile,
    ]

    print(f"[*] Running llvm-mca on {name}")
    with open(outfile, "w") as out:
        result = subprocess.run(cmd, stdout=out, stderr=subprocess.PIPE, text=True)

    if result.returncode != 0:
        print(f"[!] llvm-mca failed for {name}:\n{result.stderr}")
    else:
        print(f"    → wrote {outfile}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("asm_file")
    ap.add_argument("--outdir", default="mca")
    args = ap.parse_args()

    funcs = split_functions(args.asm_file)
    if not funcs:
        print("No functions found.")
        return

    print(f"[*] Found {len(funcs)} functions:")
    for f in funcs:
        print("   -", f)

    write_funcs(funcs, args.outdir)

    for f in funcs:
        sfile = os.path.join(args.outdir, f"{f}.s")
        run_mca(f, sfile, args.outdir)


if __name__ == "__main__":
    main()
