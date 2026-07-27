#ifndef CONFIG_H
#define CONFIG_H

void config_init(void);
const char* config_get(const char* key);
void config_set(const char* key, const char* value);
void config_cleanup(void);

#endif // CONFIG_H
