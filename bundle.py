from pathlib import Path
import re

ROOT = Path(__file__).parent

# ----------------------------
# 1. Read and clean public API
# ----------------------------
api = (ROOT / "include/cynther/cynther.h").read_text()

# Remove dev-only miniaudio include
# Assumes you wrote: #include "miniaudio/miniaudio.h" in the dev header
api = re.sub(r'#include\s+["<].*miniaudio\.h[">]', "", api)

# ----------------------------
# 2. Read external miniaudio
# ----------------------------
miniaudio = (ROOT / "external/miniaudio/miniaudio.h").read_text()

# ----------------------------
# 3. Read implementation sources
# ----------------------------
sources = [
    (ROOT / "src/audio.c").read_text(),
    (ROOT / "src/dsp.c").read_text(),
    (ROOT / "src/pattern.c").read_text(),
    (ROOT / "src/cynther.c").read_text(),
]

impl = "\n".join(sources)

# ----------------------------
# 4. Generate single-header
# ----------------------------
out = f"""{api}

#ifdef CYNTHER_IMPLEMENTATION

#define MINIAUDIO_IMPLEMENTATION
/* === bundled miniaudio === */
{miniaudio}

/* === cynther sources === */
{impl}

#endif // CYNTHER_IMPLEMENTATION
"""

(ROOT / "cynther.h").write_text(out)
print("✅ Wrote fully inlined cynther.h")
