# Toxic Text Analyzer

A comprehensive C-based system for detecting, analyzing, and reporting toxic content in multilingual text and CSV files.

## Overview

The Toxic Text Analyzer is a command-line application designed to:
- **Detect** toxic and offensive language across 12 languages
- **Analyze** text metrics (word count, uniqueness, sentence analysis)
- **Categorize** toxicity by severity levels (Mild, Moderate, Severe)
- **Compare** toxicity metrics between multiple files
- **Generate** detailed reports in text and CSV formats
- **Benchmark** sorting algorithm performance on analyzed data

## Features

### 📊 Text Analysis
- Load and process both `.txt` and `.csv` files
- Extract words, sentences, and calculate linguistic metrics
- Support for CSV column selection
- Automatic stopword filtering (removes common words)
- Track unique vs. total word counts
- Calculate lexical diversity index

### 🚨 Toxicity Detection
- **7,776 toxic terms** across 12 languages
- Detects single-word and multi-word toxic phrases
- **Severity categorization**:
  - Level 1 (Mild): Vulgar language, crude sexual terms
  - Level 2 (Moderate): Insults, derogatory terms, strong profanities
  - Level 3 (Severe): Hate speech, racist slurs, dehumanizing terms
- Case-insensitive matching
- Calculates toxicity ratio and frequency statistics

### 📈 Data Visualization & Sorting
- Sort words by: alphabetical order, frequency, or toxicity count
- Three sorting algorithms:
  - **Bubble Sort** (O(n²)) - Simple reference implementation
  - **Quick Sort** (O(n log n)) - Fast and efficient
  - **Merge Sort** (O(n log n)) - Stable sorting
- Automatic algorithm performance comparison
- Display top toxic words with frequency counts

### 🔄 Comparative Analysis
- Compare two files side-by-side
- Generate comparative reports
- Analyze differences in toxicity metrics

### 💾 Reporting
- **Text Reports**: Formatted analysis with all statistics
- **CSV Export**: Machine-readable format for further analysis
- **Bar Charts**: Visual representation of toxic word frequencies
- **Severity Breakdown**: Analysis of toxicity distribution

## Project Structure

```
├── main.c                    # Main entry point & menu system
├── data_types.h              # Global definitions & structures
├── text_processor.c          # Text normalization & processing
├── file_handler.c            # File I/O operations
├── analyzer.c                # Toxicity detection logic
├── sorting.c                 # Sorting algorithm implementations
├── reporting.c               # Report generation
├── comparative.c             # Comparative analysis
├── stopwords.txt             # Common words to exclude
├── toxic_words/              # Multilingual toxic word dictionaries
│   ├── english.txt           # 1,117 English toxic terms
│   ├── spanish.txt           # 1,195 Spanish toxic terms
│   ├── french.txt            # 1,407 French toxic terms
│   ├── german.txt            # 256 German toxic terms
│   ├── italian.txt           # 735 Italian toxic terms
│   ├── portuguese.txt        # 657 Portuguese toxic terms
│   ├── russian.txt           # 1,465 Russian toxic terms
│   ├── japanese.txt          # 290 Japanese toxic terms
│   ├── chinese.txt           # 304 Chinese toxic terms
│   ├── arabic.txt            # 135 Arabic toxic terms
│   ├── hindi.txt             # 133 Hindi toxic terms
│   ├── malay.txt             # 82 Malay toxic terms
│   └── README.md             # Dictionary documentation
├── analysis/                 # Sample files for testing
│   ├── cyberbullying_tweets.csv
│   ├── hate_asian_1k.txt
│   ├── hate_bisexual_1k.txt
│   ├── hate_black_1k.txt
│   ├── youtoxic_english_1000.csv
│   └── testing.txt
└── README.md                 # This file
```

## System Requirements

- **OS**: Windows/Linux/Mac
- **Compiler**: GCC or compatible C compiler
- **Memory**: Minimum 512MB RAM
- **Storage**: ~50MB for toxic word dictionaries

## Building & Running

### Compile
```bash
gcc -o main main.c text_processor.c file_handler.c analyzer.c sorting.c reporting.c comparative.c
```

Or use the VS Code build task (Ctrl+Shift+B on Windows).

### Run
```bash
./main
```

## Usage

### Main Menu Options

```
1. Load and analyze text file
   - Specify a .txt or .csv file from current directory or ./analysis/ folder
   - For CSV: select which columns to analyze
   - Displays: word count, unique words, toxicity ratio, severity breakdown

2. Add/manage toxic words dictionary
   - Manually add custom toxic words with severity levels
   - Useful for domain-specific terminology

3. Reload dictionaries from file
   - Reloads toxic words and stopwords from disk
   - Use after manually editing dictionary files

4. Display toxic words bar chart
   - Visual representation of top toxic words found
   - Shows frequency for each detected term

5. Analyze word frequencies (sort by algorithm)
   - Choose sorting criterion: alphabetical, frequency, or toxicity
   - Choose algorithm: Bubble Sort, Quick Sort, or Merge Sort
   - View sorted results with timing metrics

6. View severity breakdown analysis
   - Breakdown of mild, moderate, and severe toxic content
   - Percentage and count statistics

7. Compare sorting algorithms performance
   - Runs all three algorithms on same dataset
   - Compares execution time and correctness
   - Educational tool for algorithm analysis

8. Compare two files (File A vs File B)
   - Load and analyze two separate files
   - Generate side-by-side comparison report
   - Identify which file has higher toxicity

9. Save text report
   - Exports analysis to formatted .txt file
   - Includes all statistics and findings

A. Export CSV report
   - Exports analysis to machine-readable .csv format
   - Suitable for further data analysis

0. Exit program
```

## Example Workflow

```
1. Start program → Run main executable
2. Load file → Option 1 → Enter "cyberbullying_tweets.csv"
3. Select columns → Enter "1,2" to analyze specific columns
4. View results → Program displays analysis statistics
5. Sort words → Option 5 → Choose criterion and algorithm
6. Generate report → Option 9 → Save detailed analysis
7. Compare → Option 8 → Load second file for comparison
8. Export → Option A → Generate CSV for further analysis
9. Exit → Option 0
```

## Data Formats

### Input Files

**Text (.txt)**
```
This is a sample text file. It can contain multiple sentences.
Each line will be processed for toxic content analysis.
```

**CSV (.csv)**
```
username,message,date
user1,"This is a message",2024-01-01
user2,"Another message here",2024-01-02
```

### Output Reports

**Text Report**
```
========File Analysis Results=======
Total words: 1500
Unique words: 850
Total sentences: 45
Toxic occurrences: 12 (0.80%)
Average word length: 4.5
```

**CSV Report**
```
Metric,Value
Total Words,1500
Unique Words,850
Toxic Occurrences,12
Toxicity Ratio,0.80%
```

## Performance Characteristics

| Operation | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| Load file | O(n) | O(n) |
| Detect toxicity | O(n×m) | O(m) |
| Bubble Sort | O(n²) | O(1) |
| Quick Sort | O(n log n) avg | O(log n) |
| Merge Sort | O(n log n) | O(n) |
| Generate report | O(n) | O(1) |

*n = number of words, m = number of toxic terms*

## Configuration

Edit `data_types.h` to adjust system limits:

```c
#define MAX_WORDS 100000        // Max words per file
#define MAX_UNIQUE 10000        // Max unique words
#define MAX_TOXIC 10000         // Max toxic word entries
#define MAX_STOPWORDS 2000      // Max stopwords
#define MAX_TOXIC_PHRASES 2000  // Max multi-word phrases
```

## Toxic Word Dictionary

The program includes **6,310+ toxic terms** from the **NLLB-200 Toxic Words List** project by Facebook Research.

### Adding New Languages

1. Create `toxic_words/[language].txt` with format:
   ```
   word,severity
   another_word,1
   hate_term,3
   ```

2. Restart the program to automatically load the new language

### Severity Levels in Dictionary Files

- `1` = Mild (vulgar, crude)
- `2` = Moderate (insults, derogatory)
- `3` = Severe (hate speech, slurs)

## Known Limitations

- **Memory**: Limited to ~100K words per file analysis
- **CSV**: Tested with standard CSV format; complex nested structures may not parse correctly
- **Languages**: Supports only the 12 included languages
- **Performance**: Large datasets (>100K rows) may require several seconds to process

## Testing with Your Own Files

To test the analyzer with your own files, simply add them to the `./analysis/` folder:

1. Place your `.txt` or `.csv` files in the `analysis/` directory
2. Run the program and select Option 1 (Load and analyze text file)
3. Enter the filename when prompted
4. Follow the on-screen instructions to select columns (for CSV) or view analysis

The `analysis/` folder contains sample test files to get you started:
- `test_hate_asian_1000.txt`
- `test_hate_bisexual_1000.txt`
- `test_hate_black_1000.txt`
- `test_cyberbullying_1000.csv`
- `test_youtoxic_english_1000.csv`
- `test_general_data.txt`

## Troubleshooting

### "File not found" error
- Ensure file is in current directory or `./analysis/` folder
- Check filename spelling and extension

### "Memory allocation failed"
- Close other programs to free memory
- Reduce `MAX_WORDS` constant in `data_types.h`

### No toxic words detected
- Verify toxic word dictionaries are in `./toxic_words/` folder
- Use Option 3 to reload dictionaries
- Check file contains actual toxic language

### Sorting is slow
- For large unique word counts (>10K), prefer Quick Sort or Merge Sort
- Bubble Sort is O(n²) and provided as educational reference

## Contributing

To enhance the program:

1. **Add languages**: Place new `.txt` files in `toxic_words/`
2. **Improve detection**: Enhance regex patterns in `text_processor.c`
3. **Optimize sorting**: Replace algorithms in `sorting.c`
4. **Better reporting**: Add visualization in `reporting.c`

## Attribution

- **Toxic Words Data**: [NLLB-200 TWL](https://github.com/facebookresearch/flores) by Facebook Research
- **Test Data**: Various toxicity datasets from public sources
- **Academic Purpose**: Built for COMP1028 coursework

## License

Educational use. Please refer to the NLLB-200 project for licensing details on the toxic words dictionary.

## Author Notes

This program demonstrates:
- Dynamic memory management in C
- File I/O and CSV parsing
- Sorting algorithm implementations and analysis
- Data structure design for efficient toxicity detection
- Report generation and data export
- Comparative analysis techniques

---

**Version**: 1.0  
**Last Updated**: December 2025  
**Course**: COMP1028 - Programming and Algorithms (University of Nottingham Malaysia)
