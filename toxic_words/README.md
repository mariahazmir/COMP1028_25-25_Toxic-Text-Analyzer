# Toxic Words Dictionary

This directory contains multilingual toxic word lists used for content moderation and toxicity detection.

## Source

The toxic word lists for most languages were obtained from:
- **NLLB-200 Toxic Words List (NLLB-200 TWL)**
- Repository: [facebook/flores - Toxicity Module](https://github.com/facebookresearch/flores/blob/main/toxicity/README.md)
- Password-protected archive format: `[language_code]_twl.zip`

## Languages Supported

| Language | File | Source | Word Count |
|----------|------|--------|-----------|
| English | `english.txt` | NLLB-200 TWL | 1,117 |
| Spanish | `spanish.txt` | NLLB-200 TWL | 1,195 |
| French | `french.txt` | NLLB-200 TWL | 29 |
| German | `german.txt` | NLLB-200 TWL | 256 |
| Italian | `italian.txt` | NLLB-200 TWL | 735 |
| Portuguese | `portuguese.txt` | NLLB-200 TWL | 661 |
| Russian | `russian.txt` | NLLB-200 TWL | 1,465 |
| Japanese | `japanese.txt` | NLLB-200 TWL | 291 |
| Chinese | `chinese.txt` | NLLB-200 TWL | 304 |
| Arabic | `arabic.txt` | NLLB-200 TWL | 135 |
| Hindi | `hindi.txt` | NLLB-200 TWL | 133 |
| Malay | `malay.txt` | Custom | 89 |

**Total: 6,310+ toxic terms across 12 languages**

## File Format

Each `.txt` file uses the following CSV format:
```
word,severity
```

### Severity Levels

- **1 (Mild)**: Vulgar language, crude sexual terms, mild insults
- **2 (Moderate)**: Strong insults, derogatory terms, strong profanities
- **3 (Severe)**: Hate speech, racist slurs, dehumanizing terms, extreme profanities

## Usage

The program dynamically loads all `.txt` files from this directory on startup. To add a new language:

1. Create a new file: `[language_name].txt`
2. Use the format: `word,severity` (one per line)
3. Restart the program to load the new language

## License & Attribution

Please refer to the [original repository](https://github.com/facebookresearch/flores) for licensing details and proper attribution.

## Notes

- Multi-word phrases are supported (e.g., "go to hell")
- Case-insensitive matching is performed during analysis
- Severity indicators help contextualize detection results
