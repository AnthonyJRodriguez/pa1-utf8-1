#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int32_t is_ascii(char str[]) {
    int i = 0;
    bool TF = true;
    while (str[i] != '\0') {
        if ((unsigned char)str[i] > 127) {
            TF = false;
            break;
        }
        i++;
    }
    if (TF){
        return 1; // is ASCII
    }
    else{
        return 0; // is not ASCII
    }
}

int32_t capitalize_ascii(char str[]) {
    int32_t count = 0; 
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32; // Converts to uppercase
            count++; 
        }
        i++; 
    }
    return count; 
}

int32_t width_from_start_byte(char start_byte){
    unsigned char byte = (unsigned char) start_byte;
    if ((byte & 0x80) == 0x00) {
        return 1;
    }
    else if ((byte & 0xE0) == 0xC0) {
        return 2;
    }
    else if ((byte & 0xF0) == 0xE0) {
        return 3;
    }
    else if ((byte & 0xF8) == 0xF0) {
        return 4;
    }
    return -1;
}

int32_t utf8_strlen(char str[]){
    int32_t length = 0;
    int32_t i = 0;
    while (str[i] != '\0'){
        int32_t width = (width_from_start_byte(str[i]));
        if (width == -1){
            return -1;
        }
        i += width;
        length++;
    }
    return length;
}

int32_t codepoint_index_to_byte_index(char str[], int32_t cpi){
    int32_t codepoint_index = 0;
    int32_t byte_index = 0;
    while (str[byte_index] != '\0'){
        int32_t width = width_from_start_byte(str[byte_index]);
        if (width == -1){
            return -1;
        }
        if (codepoint_index == cpi){
            return byte_index;
        }
        byte_index += width;
        codepoint_index ++;
    }
    return -1;
}

void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]){
    if (cpi_start >= cpi_end || cpi_start < 0 || cpi_end < 0){
        return;
    }
    int32_t cpi_index = 0;
    int32_t byte_index = 0;
    int32_t result_index = 0;
    while (str[byte_index] != '\0'){
        int32_t width = width_from_start_byte(str[byte_index]);
        if (width == -1){
            return;
        }
        if (cpi_index >=cpi_start && cpi_index < cpi_end){
            for (int32_t x = 0; x < width; x++){
                result[result_index] = str[byte_index+x];
                result_index++;
            }
        }
        byte_index += width;
        cpi_index++;
    }
    result[result_index] = '\0';
}

int32_t codepoint_at(char str[], int32_t cpi){
    int32_t cpi_index = 0;  // Codepoint index tracker
    int32_t byte_index = 0; // Byte index tracker

    // Traverse through the UTF-8 string
    while (str[byte_index] != '\0') {
        int32_t width = width_from_start_byte(str[byte_index]);
        if (width == -1) {
            return -1; // Error if width is invalid
        }

        // If the current codepoint index matches the desired index
        if (cpi_index == cpi) {
            int32_t codepoint = 0;

            // Extract codepoint based on width
            if (width == 1) {
                codepoint = str[byte_index] & 0x7F; // 1-byte UTF-8 (ASCII)
            } else if (width == 2) {
                codepoint = ((str[byte_index] & 0x1F) << 6) |
                            (str[byte_index + 1] & 0x3F); // 2-byte UTF-8
            } else if (width == 3) {
                codepoint = ((str[byte_index] & 0x0F) << 12) |
                            ((str[byte_index + 1] & 0x3F) << 6) |
                            (str[byte_index + 2] & 0x3F); // 3-byte UTF-8
            } else if (width == 4) {
                codepoint = ((str[byte_index] & 0x07) << 18) |
                            ((str[byte_index + 1] & 0x3F) << 12) |
                            ((str[byte_index + 2] & 0x3F) << 6) |
                            (str[byte_index + 3] & 0x3F); // 4-byte UTF-8
            }
            return codepoint; // Return the calculated codepoint value
        }

        // Move to the next codepoint
        byte_index += width;
        cpi_index++;
    }

    return -1; // Return -1 if cpi index is out of range
}

int32_t codepoint_at(char str[], int32_t cpi);

// Function to check if the codepoint at the specified index is an animal emoji
char is_animal_emoji_at(char str[], int32_t cpi) {
    int32_t byte_index = codepoint_index_to_byte_index(str, cpi);
    
    if (byte_index < 0) {
        return 0; // Not an animal emoji
    }

    int32_t codepoint = codepoint_at(str, cpi);
    
    // Check if the codepoint is within the specified animal emoji ranges
    if ((codepoint >= 0x1F400 && codepoint <= 0x1F43F) || // 🐀 to 🐿️
        (codepoint >= 0x1F980 && codepoint <= 0x1F993)) { // 🦀 to 🦮
        return (char)codepoint; // Return the emoji as a char
    }

    return 0; // Not an animal emoji
}

void next_utf8_char(char str[], int32_t cpi, char result[]) {
    int32_t codepoint = 0;
    int32_t cpi_index = 0;
    int32_t byte_index = 0;

    // Calculate the codepoint at the specified index
    while (str[byte_index] != '\0') {
        int32_t width = 0;
        unsigned char start_byte = str[byte_index];

        // Determine the width of the character
        if ((start_byte & 0x80) == 0x00) width = 1;
        else if ((start_byte & 0xE0) == 0xC0) width = 2;
        else if ((start_byte & 0xF0) == 0xE0) width = 3;
        else if ((start_byte & 0xF8) == 0xF0) width = 4;
        else break;

        if (cpi_index == cpi) {
            // Decode the current codepoint based on its byte width
            if (width == 1) codepoint = str[byte_index] & 0x7F;
            else if (width == 2)
                codepoint = ((str[byte_index] & 0x1F) << 6) | (str[byte_index + 1] & 0x3F);
            else if (width == 3)
                codepoint = ((str[byte_index] & 0x0F) << 12) |
                            ((str[byte_index + 1] & 0x3F) << 6) |
                            (str[byte_index + 2] & 0x3F);
            else if (width == 4)
                codepoint = ((str[byte_index] & 0x07) << 18) |
                            ((str[byte_index + 1] & 0x3F) << 12) |
                            ((str[byte_index + 2] & 0x3F) << 6) |
                            (str[byte_index + 3] & 0x3F);
            break;
        }

        byte_index += width;
        cpi_index++;
    }

    // Increment the codepoint
    codepoint += 1;

    // Encode the incremented codepoint back to UTF-8
    if (codepoint <= 0x7F) {
        result[0] = (char)codepoint;
        result[1] = '\0';
    } else if (codepoint <= 0x7FF) {
        result[0] = (char)(0xC0 | ((codepoint >> 6) & 0x1F));
        result[1] = (char)(0x80 | (codepoint & 0x3F));
        result[2] = '\0';
    } else if (codepoint <= 0xFFFF) {
        result[0] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
        result[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        result[2] = (char)(0x80 | (codepoint & 0x3F));
        result[3] = '\0';
    } else if (codepoint <= 0x10FFFF) {
        result[0] = (char)(0xF0 | ((codepoint >> 18) & 0x07));
        result[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        result[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        result[3] = (char)(0x80 | (codepoint & 0x3F));
        result[4] = '\0';
    }
}








int main(){   
    char str[100];
    printf("Enter a UTF-8 encoded string: ");
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = 0;


    printf("\nValid ASCII: %s\n", is_ascii(str) ? "true" : "false");
    
    char upper_str[100];
    strcpy(upper_str, str);
    capitalize_ascii(upper_str);
    printf("Uppercased ASCII: \"%s\"\n", upper_str);
    

    int length_bytes = strlen(str);
    printf("Length in bytes: %d\n", length_bytes);

    int num_codepoints = utf8_strlen(str);
    printf("Number of code points: %d\n", num_codepoints);
    
    printf("Bytes per code point: ");
    for (int i = 0; i < num_codepoints; i++) {
        int32_t byte_count = width_from_start_byte(str[codepoint_index_to_byte_index(str, i)]);
        printf("%d ", byte_count);
    }
    printf("\n");

    char substring[100];
    utf8_substring(str, 0, 6, substring);
    printf("Substring of the first 6 code points: \"%s\"\n", substring);

    printf("Code points as decimal numbers: ");
    for (int i = 0; i < num_codepoints; i++) {
        printf("%d ", codepoint_at(str, i));
    }
    printf("\n");

    printf("Animal emojis: ");
    bool found_animal = false;

    char emoji_buffer[5];

    for (int i = 0; i < num_codepoints; i++) {
        if (is_animal_emoji_at(str, i)) {
            utf8_substring(str, i, i + 1, emoji_buffer); 
            printf("%s ", emoji_buffer); 
            found_animal = true;
        }
    }

    if (!found_animal) {
        printf("None");
    }
    printf("\n");

    char next_char[5]; // Max 4 bytes + null terminator
    int32_t index_to_check = 3; // Change to any index you'd like
    next_utf8_char(str, index_to_check, next_char);
    printf("Next Character of Codepoint at Index %d: %s\n", index_to_check, next_char);

    return 0;

}
    

