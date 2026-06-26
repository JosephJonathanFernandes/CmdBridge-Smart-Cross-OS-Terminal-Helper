#include "commands.h"
#include <string.h>

int load_command_templates(CommandTemplate *templates) {
    int count = 0;

    // 1. find files
    strcpy(templates[count].action, "find");
    strcpy(templates[count].object, "files");
    strcpy(templates[count].linux_cmd, "find . -name \"*{TARGET}*\"");
    strcpy(templates[count].windows_cmd, "Get-ChildItem -Recurse -Filter *{TARGET}*");
    strcpy(templates[count].mac_cmd, "find . -name \"*{TARGET}*\"");
    count++;

    // 2. show running processes
    strcpy(templates[count].action, "show");
    strcpy(templates[count].object, "processes");
    strcpy(templates[count].linux_cmd, "ps aux");
    strcpy(templates[count].windows_cmd, "tasklist");
    strcpy(templates[count].mac_cmd, "ps aux");
    count++;

    // 3. create folder
    strcpy(templates[count].action, "create");
    strcpy(templates[count].object, "folder");
    strcpy(templates[count].linux_cmd, "mkdir \"{TARGET}\"");
    strcpy(templates[count].windows_cmd, "mkdir \"{TARGET}\"");
    strcpy(templates[count].mac_cmd, "mkdir \"{TARGET}\"");
    count++;
    
    // 4. list files
    strcpy(templates[count].action, "list");
    strcpy(templates[count].object, "files");
    strcpy(templates[count].linux_cmd, "ls -la");
    strcpy(templates[count].windows_cmd, "dir");
    strcpy(templates[count].mac_cmd, "ls -la");
    count++;

    // 5. delete folder
    strcpy(templates[count].action, "delete");
    strcpy(templates[count].object, "folder");
    strcpy(templates[count].linux_cmd, "rm -r \"{TARGET}\"");
    strcpy(templates[count].windows_cmd, "Remove-Item -Recurse -Force \"{TARGET}\"");
    strcpy(templates[count].mac_cmd, "rm -r \"{TARGET}\"");
    count++;

    // 6. move file
    strcpy(templates[count].action, "move");
    strcpy(templates[count].object, "file");
    strcpy(templates[count].linux_cmd, "mv \"{TARGET}\" \"{DEST}\""); // Need special handling if TARGET has dest
    strcpy(templates[count].windows_cmd, "Move-Item \"{TARGET}\" \"{DEST}\"");
    strcpy(templates[count].mac_cmd, "mv \"{TARGET}\" \"{DEST}\"");
    count++;

    // 7. copy file
    strcpy(templates[count].action, "copy");
    strcpy(templates[count].object, "file");
    strcpy(templates[count].linux_cmd, "cp \"{TARGET}\" \"{DEST}\"");
    strcpy(templates[count].windows_cmd, "Copy-Item \"{TARGET}\" \"{DEST}\"");
    strcpy(templates[count].mac_cmd, "cp \"{TARGET}\" \"{DEST}\"");
    count++;

    // 8. compress files
    strcpy(templates[count].action, "compress");
    strcpy(templates[count].object, "files");
    strcpy(templates[count].linux_cmd, "tar -czvf archive.tar.gz \"{TARGET}\"");
    strcpy(templates[count].windows_cmd, "Compress-Archive -Path \"{TARGET}\" -DestinationPath archive.zip");
    strcpy(templates[count].mac_cmd, "tar -czvf archive.tar.gz \"{TARGET}\"");
    count++;

    // 9. search text
    strcpy(templates[count].action, "search");
    strcpy(templates[count].object, "text");
    strcpy(templates[count].linux_cmd, "grep -r \"{TARGET}\" .");
    strcpy(templates[count].windows_cmd, "Select-String -Path * -Pattern \"{TARGET}\" -AllMatches");
    strcpy(templates[count].mac_cmd, "grep -r \"{TARGET}\" .");
    count++;

    // 10. show disk usage
    strcpy(templates[count].action, "show");
    strcpy(templates[count].object, "disk");
    strcpy(templates[count].linux_cmd, "df -h");
    strcpy(templates[count].windows_cmd, "Get-PSDrive -PSProvider FileSystem");
    strcpy(templates[count].mac_cmd, "df -h");
    count++;

    return count;
}
