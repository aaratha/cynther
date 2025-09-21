from pathlib import Path
import re

ROOT = Path(__file__).parent

# ----------------------------
# 0. Read LICENSE.txt
# ----------------------------
license_text = (ROOT / "LICENSE.txt").read_text().strip()
license_block = (
    "/*\n" + "\n".join("    " + line for line in license_text.splitlines()) + "\n*/\n\n"
)

# ----------------------------
# 1. Read and clean public API
# ----------------------------
api = (ROOT / "include/cynther/cynther.h").read_text()
api = re.sub(r'#include\s+["<][^">]*miniaudio\.h[">]\s*\n', "", api)

# ----------------------------
# 2. Read external miniaudio
# ----------------------------
miniaudio = (ROOT / "external/miniaudio/miniaudio.h").read_text().strip()

# ----------------------------
# 3. Read implementation sources
# ----------------------------
sources = [
    (ROOT / "src/audio.c").read_text().strip(),
    (ROOT / "src/dsp.c").read_text().strip(),
    (ROOT / "src/pattern.c").read_text().strip(),
    (ROOT / "src/effect.c").read_text().strip(),
    (ROOT / "src/cynther.c").read_text().strip(),
]
impl = "\n\n".join(sources)

# ----------------------------
# 4. Generate single-header
# ----------------------------
out = f"""{license_block}#pragma once
#define MINIAUDIO_IMPLEMENTATION
{miniaudio}

/* === cynther API === */
{api}

#ifdef CYNTHER_IMPLEMENTATION
/* === cynther sources === */
{impl}
#endif
"""

(ROOT / "cynther.h").write_text(out)
print("✅ Wrote fully inlined cynther.h")
