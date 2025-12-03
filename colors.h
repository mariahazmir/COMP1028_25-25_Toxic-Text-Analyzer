/*
================================================================================
 COLORS.H - ANSI COLOR DEFINITIONS FOR UI
 
 Purpose: Define ANSI color codes for enhanced terminal UI
 
 Features:
  - Blue and green color palette with multiple shades
  - Easy-to-use macros for consistent styling
================================================================================
*/

#ifndef COLORS_H
#define COLORS_H

// ANSI Color Codes
#define RESET           "\x1b[0m"
#define BOLD            "\x1b[1m"
#define DIM             "\x1b[2m"

// Blue Shades
#define DARK_BLUE       "\x1b[34m"
#define BRIGHT_BLUE     "\x1b[94m"
#define LIGHT_BLUE      "\x1b[36m"   // Cyan (light blue)

// Green Shades
#define DARK_GREEN      "\x1b[32m"
#define BRIGHT_GREEN    "\x1b[92m"
#define LIGHT_GREEN     "\x1b[38;5;120m"  // Lighter green

// Text Styles
#define HEADER          "\x1b[94m\x1b[1m"        // Bold bright blue
#define MENU_LABEL      "\x1b[36m"              // Light blue (cyan)
#define OPTION_NUM      "\x1b[92m"              // Bright green
#define PROMPT          "\x1b[38;5;51m"         // Bright cyan
#define SUCCESS         "\x1b[92m"              // Bright green
#define WARNING         "\x1b[38;5;226m"        // Yellow-green
#define ERROR           "\x1b[34m"              // Dark blue for errors
#define INFO            "\x1b[36m"              // Light blue for info
#define STAT_LABEL      "\x1b[32m"              // Dark green for labels
#define STAT_VALUE      "\x1b[92m"              // Bright green for values
#define DIVIDER         "\x1b[38;5;24m"         // Dark blue for dividers

#endif
