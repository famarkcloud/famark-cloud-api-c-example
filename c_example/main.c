#include <stdio.h>
#include "FamarkCloudAPI-C.h"
#include <json-c/json.h>

int main()
{
    char domainName[100], userName[100], password[100];
    printf("\nDomain Name:");
    scanf_s("%99s", domainName, 100);
    printf("\nUser Name:");
    scanf_s("%99s", userName, 100);
    printf("\nPassword:");
    scanf_s("%99s", password, 100);
    char credData[1000] = "{\"DomainName\": \"";
    int credSize = sizeof(credData);
    strcat_s(credData, credSize, domainName);
    strcat_s(credData, credSize, "\", \"UserName\": \"");
    strcat_s(credData, credSize, userName);
    strcat_s(credData, credSize, "\", \"Password\": \"");
    strcat_s(credData, credSize, password);
    strcat_s(credData, credSize, "\"}");

    //Connect action call
    char* sessionData = famark_api_post_data("/Credential/Connect", credData, NULL);

    struct json_object* sessionObject = json_tokener_parse(sessionData);
    const char* sessionId = json_object_get_string(sessionObject);

    if (sessionId) {
        printf("Session Id: %s\n", sessionId);
    }
    else {
        return -1;
    }

    printf("Do you want to create new record (y/n) \n");
    char userResp = getch();
    //clearing new lines from input buffer
    while (getchar() != '\n');

    while (userResp == 'y' || userResp == 'Y')
    {
        char displayName[100], systemName[100];
        printf("\nDisplay Name:");
        fgets(displayName, 100, stdin);
        displayName[strcspn(displayName, "\n")] = 0;
        printf("\nSystemName:");
        fgets(systemName, 100, stdin);
        systemName[strcspn(systemName, "\n")] = 0;
        char profileData[1000] = "{\"DisplayName\": \"";
        strcat_s(profileData, sizeof(profileData), displayName);
        strcat_s(profileData, sizeof(profileData), "\", \"SystemName\": \"");
        strcat_s(profileData, sizeof(profileData), systemName);
        strcat_s(profileData, sizeof(profileData), "\"}");

        //Calling CreateRecord action on System_Profile entity of System solution
        char* recordData = famark_api_post_data("/System_Profile/CreateRecord", profileData, sessionId);

        struct json_object* recordObject = json_tokener_parse(recordData);
        const char* recordId = json_object_get_string(recordObject);

        free(recordData);

        if (recordId) {
            printf("Created Record Id: %s\n", recordId);
            //Login to your famark cloud domain to see the newly created record under System => Profile
            printf("Do you want to create more profile record (y/n)? \n");
        }
        else {
            printf("Do you want to try again (y/n)? \n");
        }
        userResp = getch();
    }

    //Calling RetrieveMultipleRecords action on the Hospital_Doctor entity
    char data[] = "{\"Columns\": \"DisplayName,SystemName\"}";
    char* records = famark_api_post_data("/System_Profile/RetrieveMultipleRecords", data, sessionId);
    struct json_object* recordObjects = json_tokener_parse(records);

    int numRecords = json_object_array_length(recordObjects);
    printf("\nNumber of Records = %d\n", numRecords);

    struct json_object* tmp;
    for (int i = 0; i < numRecords; i++) {
        // Set in tmp the json_object of the secu_code array at index i
        tmp = json_object_array_get_idx(recordObjects, i);
        printf("%d ", i + 1);
        printf("%s\n", json_object_to_json_string(tmp));
    }

    free(records);

    return 0;
}