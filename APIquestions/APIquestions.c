/*
 * Auteur : Bob LENGLET
 * Date : 29 Janvier 2026
 * But : Fonctions pour fetch les questions en ligne
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "APIquestions.h"

#define QUESTION_COUNT 10
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define API_URL "https://opentdb.com/api.php?amount=" STR(QUESTION_COUNT)

struct MemoryStruct {
    char *memory;
    size_t size;
};

static Question questions[QUESTION_COUNT];
static int current_index = 0;
static int is_initialized = 0;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

static char* url_decode(const char* str) {
    if (str == NULL) return NULL;
    int out_len = 0;
    char* decoded = curl_easy_unescape(NULL, str, 0, &out_len);
    return decoded;
}

static char* html_entity_decode(const char* str) {
    if (str == NULL) return strdup("Unknown");
    
    size_t len = strlen(str);
    char* decoded = malloc(len + 1);
    if (!decoded) return strdup("Unknown");
    
    const char* src = str;
    char* dst = decoded;
    
    while (*src) {
        if (*src == '&') {
            if (src[1] == '#') {
                char* endptr = NULL;
                long code = strtol(src + 2, &endptr, 10);
                if (endptr && *endptr == ';') {
                    if (code > 0 && code < 256) {
                        *dst++ = (char)code;
                    }
                    src = endptr + 1;
                    continue;
                }
            }
            
            if (strncmp(src, "&quot;", 6) == 0) {
                *dst++ = '"'; src += 6; continue;
            }
            if (strncmp(src, "&amp;", 5) == 0) {
                *dst++ = '&'; src += 5; continue;
            }
            if (strncmp(src, "&lt;", 4) == 0) {
                *dst++ = '<'; src += 4; continue;
            }
            if (strncmp(src, "&gt;", 4) == 0) {
                *dst++ = '>'; src += 4; continue;
            }
            if (strncmp(src, "&apos;", 6) == 0) {
                *dst++ = '\''; src += 6; continue;
            }
            if (strncmp(src, "&ldquo;", 7) == 0) {
                *dst++ = '"'; src += 7; continue;
            }
            if (strncmp(src, "&rdquo;", 7) == 0) {
                *dst++ = '"'; src += 7; continue;
            }
            if (strncmp(src, "&lsquo;", 7) == 0) {
                *dst++ = '\''; src += 7; continue;
            }
            if (strncmp(src, "&rsquo;", 7) == 0) {
                *dst++ = '\''; src += 7; continue;
            }
            if (strncmp(src, "&ndash;", 7) == 0) {
                *dst++ = '-'; src += 7; continue;
            }
            
            *dst++ = *src++;
        } else {
            *dst++ = *src++;
        }
    }
    
    *dst = '\0';
    return decoded;
}

static char* strdup_json(cJSON *item) {
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        char* url_decoded = url_decode(item->valuestring);
        const char* to_decode = url_decoded ? url_decoded : item->valuestring;
        char* html_decoded = html_entity_decode(to_decode);
        if (url_decoded) curl_free(url_decoded);
        return html_decoded;
    }
    return strdup("Unknown");
}

int q_init_library(void) {
    srand(time(NULL));
    
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    
    chunk.memory = malloc(1);
    chunk.size = 0;
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    
    if (!curl_handle) {
        fprintf(stderr, "[API Error] Impossible d'initialiser CURL.\n");
        return -1;
    }
    
    const char *difficulties[] = {"easy", "medium", "hard"};
    int question_counts[] = {3, 4, 3};
    int question_offset = 0;
    
    for (int d = 0; d < 3; d++) {
        char url[256];
        snprintf(url, sizeof(url), "https://opentdb.com/api.php?amount=%d&difficulty=%s&encode=url3986", question_counts[d], difficulties[d]);
        
        // Reset chunk for each request
        if (d > 0) {
            free(chunk.memory);
            chunk.memory = malloc(1);
            chunk.size = 0;
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        // FIX: Force HTTP/1.1 to avoid HTTP/2 framing issues
        curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        
        // Add timeout to avoid hanging
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10L);
        
        printf("[API] Downloading questions from: %s\n", url);
        
        res = curl_easy_perform(curl_handle);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "[API Error] curl failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            curl_easy_cleanup(curl_handle);
            curl_global_cleanup();
            return -1;
        }
        
        cJSON *json = cJSON_Parse(chunk.memory);
        if (json == NULL) {
            fprintf(stderr, "[API Error] JSON Parse failed\n");
            free(chunk.memory);
            curl_easy_cleanup(curl_handle);
            curl_global_cleanup();
            return -1;
        }
        
        cJSON *results = cJSON_GetObjectItemCaseSensitive(json, "results");
        cJSON *item = NULL;
        
        cJSON_ArrayForEach(item, results) {
            if (question_offset >= QUESTION_COUNT) break;
            
            questions[question_offset].category   = strdup_json(cJSON_GetObjectItem(item, "category"));
            questions[question_offset].difficulty = strdup_json(cJSON_GetObjectItem(item, "difficulty"));
            questions[question_offset].question   = strdup_json(cJSON_GetObjectItem(item, "question"));
            
            char *correct_str = strdup_json(cJSON_GetObjectItem(item, "correct_answer"));
            cJSON *incorrects = cJSON_GetObjectItem(item, "incorrect_answers");
            
            int wrong_count = cJSON_GetArraySize(incorrects);
            int total_answers = wrong_count + 1;
            questions[question_offset].answer_count = total_answers;
            
            questions[question_offset].answers = malloc(sizeof(char*) * total_answers);
            questions[question_offset].answers[0] = correct_str;
            
            int j = 1;
            cJSON *wrong_ans = NULL;
            cJSON_ArrayForEach(wrong_ans, incorrects) {
                questions[question_offset].answers[j] = strdup_json(wrong_ans);
                j++;
            }
            
            // Shuffle
            for (int i = total_answers - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                char *temp = questions[question_offset].answers[i];
                questions[question_offset].answers[i] = questions[question_offset].answers[j];
                questions[question_offset].answers[j] = temp;
            }
            
            // Find correct option
            for (int i = 0; i < total_answers; i++) {
                if (strcmp(questions[question_offset].answers[i], correct_str) == 0) {
                    questions[question_offset].correct_option = 'A' + i;
                    break;
                }
            }
            
            question_offset++;
        }
        
        cJSON_Delete(json);
        
        // Rate limiting
        if (d < 2) {
            printf("[API] Waiting 5 seconds to respect API rate limit...\n");
            sleep(5);
        }
    }
    
    free(chunk.memory);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    
    is_initialized = 1;
    printf("[API] Successfully loaded %d questions!\n", question_offset);
    
    return 0;
}

Question* q_pull_question(void) {
    if (!is_initialized) {
        fprintf(stderr, "[API Error] Library not initialized. Call q_init_library() first.\n");
        return NULL;
    }
    
    if (current_index >= QUESTION_COUNT) {
        return NULL;
    }
    
    return &questions[current_index++];
}

void q_debug_dump_all(void) {
    if (!is_initialized) {
        printf("[API] Not initialized yet.\n");
        return;
    }
    
    printf("\n========== DATABASE DUMP ==========\n");
    for (int i = 0; i < QUESTION_COUNT; i++) {
        printf("\n[Q%d] %s\n", i+1, questions[i].question);
        printf("     Category: %s | Difficulty: %s\n", questions[i].category, questions[i].difficulty);
        for (int j = 0; j < questions[i].answer_count; j++) {
            char marker = (('A' + j) == questions[i].correct_option) ? '*' : ' ';
            printf("     %c [%c] %s\n", marker, 'A'+j, questions[i].answers[j]);
        }
    }
    printf("\n===================================\n");
}

void q_free_library(void) {
    for (int i = 0; i < QUESTION_COUNT; i++) {
        free(questions[i].category);
        free(questions[i].difficulty);
        free(questions[i].question);
        
        for (int j = 0; j < questions[i].answer_count; j++) {
            free(questions[i].answers[j]);
        }
        free(questions[i].answers);
    }
    
    is_initialized = 0;
    current_index = 0;
}
