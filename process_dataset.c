//滤波测试
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../app/emg_filter.h"

// Buffer size for reading lines
#define LINE_BUFFER_SIZE 4096

void print_usage(const char* prog_name) {
    printf("Usage: %s <input_csv> <output_csv> <sample_rate> [column_index]\n", prog_name);
    printf("Example: %s dataset.csv filtered.csv 1000 1\n", prog_name);
    printf("  column_index: 0-based index of the data column (default: 0)\n");
}

// Simple CSV parser to get the Nth column
int get_csv_column(char *line, int col_index, float *val) {
    char *token;
    char *rest = line;
    int current_col = 0;

    while ((token = strtok_s(rest, ",", &rest))) {
        if (current_col == col_index) {
            char *endptr;
            *val = strtof(token, &endptr);
            if (token == endptr) return 0; // Failed conversion
            return 1; // Success
        }
        current_col++;
    }
    return 0; // Column not found
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    float sample_rate = atof(argv[3]);
    int target_col = 0;
    
    if (argc >= 5) {
        target_col = atoi(argv[4]);
    }

    if (sample_rate <= 0.0f) {
        printf("Error: Invalid sample rate\n");
        return 1;
    }

    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        printf("Error: Could not open input file %s\n", input_path);
        return 1;
    }

    FILE *fout = fopen(output_path, "w");
    if (!fout) {
        printf("Error: Could not open output file %s\n", output_path);
        fclose(fin);
        return 1;
    }

    // Initialize Filters
    BiquadFilter notch, highpass, lowpass;
    Filter_Init(&notch);
    Filter_Init(&highpass);
    Filter_Init(&lowpass);

    // 1. Notch at 50Hz (mains hum)
    Filter_CalcNotch(&notch, sample_rate, 50.0f, 5.0f);
    // 2. High Pass at 20Hz (motion artifacts)
    Filter_CalcHighPass(&highpass, sample_rate, 20.0f, 0.707f);
    // 3. Low Pass at 450Hz (noise)
    Filter_CalcLowPass(&lowpass, sample_rate, 450.0f, 0.707f);

    printf("Processing %s -> %s\n", input_path, output_path);
    printf("Sample Rate: %.1f Hz, Target Column: %d\n", sample_rate, target_col);
    printf("Filters: Notch(50Hz), HP(20Hz), LP(450Hz)\n");

    // Write CSV Header
    fprintf(fout, "Time,Raw,Filtered\n");

    char line[LINE_BUFFER_SIZE];
    // Copy of line for strtok because it modifies string
    char line_copy[LINE_BUFFER_SIZE];
    
    int line_num = 0;
    int processed_samples = 0;

    while (fgets(line, sizeof(line), fin)) {
        line_num++;
        
        // Skip empty lines
        if (strlen(line) < 2) continue;

        strcpy(line_copy, line);
        
        float raw_val;
        if (!get_csv_column(line_copy, target_col, &raw_val)) {
            // Failed to parse, assume header or malformed line
            if (line_num == 1) printf("Skipping header or invalid line 1...\n");
            continue;
        }

        // Apply Filters
        float signal = raw_val;
        signal = Filter_Process(&notch, signal);
        signal = Filter_Process(&highpass, signal);
        signal = Filter_Process(&lowpass, signal);

        // Calculate time
        float time = (float)processed_samples / sample_rate;

        fprintf(fout, "%.4f,%.4f,%.4f\n", time, raw_val, signal);
        processed_samples++;
    }

    fclose(fin);
    fclose(fout);

    printf("Done. Processed %d samples.\n", processed_samples);
    return 0;
}
