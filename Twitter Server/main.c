#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ftw.h>
#include <time.h>
#include <dirent.h>
#include "cJSON.h"

#define PORT 12345
#define MAX 10000

void initialize();

void receiveData(char *data);

void sendData(char *data);

int signup(char *username, char *password);

int login(char *username, char *password);

void dataHandling(char *data);

void makeDirectories();

void generateToken();

int getTweets(char *data);

int logout(char *data);

char *getTime();

int refresh(char *data);

int like(char *data);

int comment(char *data);

int search(char *data);

int unfollow(char *data);

int follow(char *data);

int tweetProfile(char *data);

int setBio(char *data);

int changePassword(char *data);

void sortInDescendingOrder(int number[], int size);

int deleteTweet(char *data);

int fuzzySearch(char *data);

int isSubstring(char s1[], char *s2);

int tweetSearch(char *data);

char Token[33];
int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char current_file_name[100], USERNAME[15];

int main() {
    char data[1024] = {0};
    initialize();
    makeDirectories();
    while (1) {
        bzero(data, sizeof(char) * 1024);
        receiveData(data);
        dataHandling(data);
    }
}

void initialize() {

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void receiveData(char *data) {
    if (listen(server_fd, 3) < 0) {
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    read(new_socket, data, 1024);
}

void sendData(char *data) {
    send(new_socket, data, strlen(data), 0);
    printf("%s | To client: %s\n", getTime(), data);
}

void dataHandling(char *data) {
    char access[10];
    sscanf(data, "%s", access);
    if (!strcmp(access, "login")) {
        printf("%s | From client: %s", getTime(), data);
        char username[15], password[15];
        sscanf(data, "%*s %s %s", username, password);
        username[strlen(username) - 1] = '\0';

        if (login(username, password)) {
        }
    } else if (!strcmp(access, "signup")) {
        printf("%s | From client: %s", getTime(), data);
        char username[15], password[15];
        sscanf(data, "%*s %s %s", username, password);
        username[strlen(username) - 1] = '\0';
        signup(username, password);
    } else if (!strcmp(access, "send")) {
        data[strlen(data) - 1] = '\0';
        printf("%s | From client: %s", getTime(), data);
        getTweets(data);
    } else if (!strcmp(access, "logout")) {
        printf("%s | From client: %s", getTime(), data);
        logout(data);
    } else if (!strcmp(access, "refresh")) {
        printf("%s | From client: %s", getTime(), data);
        refresh(data);
    } else if (!strcmp(access, "like")) {
        printf("%s | From client: %s", getTime(), data);
        like(data);
    } else if (!strcmp(access, "comment")) {
        data[strlen(data) - 1] = '\0';
        printf("%s | From client: %s", getTime(), data);
        comment(data);
    } else if (!strcmp(access, "search")) {
        printf("%s | From client: %s", getTime(), data);
        search(data);
    } else if (!strcmp(access, "unfollow")) {
        printf("%s | From client: %s", getTime(), data);
        unfollow(data);
    } else if (!strcmp(access, "follow")) {
        printf("%s | From client: %s", getTime(), data);
        follow(data);
    } else if (!strcmp(access, "profile")) {
        printf("%s | From client: %s", getTime(), data);
        tweetProfile(data);
    } else if (!strcmp(access, "set")) {
        data[strlen(data) - 1] = '\0';
        printf("%s | From client: %s", getTime(), data);
        setBio(data);
    } else if (!strcmp(access, "change")) {
        printf("%s | From client: %s", getTime(), data);
        changePassword(data);
    } else if (!strcmp(access, "delete")) {
        printf("%s | From client: %s", getTime(), data);
        deleteTweet(data);
    } else if (!strcmp(access, "fsearch")) {
        printf("%s | From client: %s", getTime(), data);
        fuzzySearch(data);
    }else if (!strcmp(access, "tsearch")) {
        printf("%s | From client: %s", getTime(), data);
        tweetSearch(data);
    }
}

void makeDirectories() {
    if (mkdir("Resources", 0777) != -1)
        printf("%s | Directory created\n", getTime());
    if (mkdir("Resources/Users", 0777) != -1)
        printf("%s | Directory created\n", getTime());
    if (mkdir("Resources/Tweets", 0777) != -1)
        printf("%s | Directory created", getTime());
}

void generateToken() {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_123456789";
    for (int i = 0; i < 32; ++i) {
        Token[i] = alphanum[rand() % (strlen(alphanum) - 1)];
    }

    Token[32] = '\0';
}

int signup(char *username, char *password) {
    FILE *file_pointer;
    char filename[200] = "Resources/Users/";
    strcat(filename, username);
    strcat(filename, ".user.json");
    if (fopen(filename, "r") != NULL) {
        printf("%s | This username already exists.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"This username is already taken.\"}");
        return 0;
    }
    cJSON *user;
    user = cJSON_CreateObject();

    cJSON_AddItemToObject(user, "username", cJSON_CreateString(username));
    cJSON_AddItemToObject(user, "password", cJSON_CreateString(password));
    cJSON_AddItemToObject(user, "bio", cJSON_CreateString(""));

    cJSON_AddArrayToObject(user, "followers");
    cJSON_AddArrayToObject(user, "followings");
    cJSON_AddArrayToObject(user, "personalTweets");

    char *out = cJSON_PrintUnformatted(user);
    file_pointer = fopen(filename, "w");
    if (strlen(out) > 0) {

        fputs(out, file_pointer);
        fputs("\n", file_pointer);
    }
    fclose(file_pointer);
    sendData("{\"type\":\"Successful\",\"message\":\"\"}");
    return 1;
}

int login(char *username, char *password) {
    if (strlen(Token) != 0) {
        printf("%s | User %s already logged in.\n", getTime(), username);
        sendData("{\"type\":\"Error\",\"message\":\"User already logged in.\"}");
        return 0;
    }
    FILE *file_pointer;
    char filename[200] = "Resources/Users/";
    strcat(filename, username);
    strcat(filename, ".user.json");
    strcpy(current_file_name, filename);
    file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        printf("%s | This username does not exist.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"This username is not valid.\"}");
        return 0;
    }
    char user_information[MAX] = {0};
    fgets(user_information, MAX, file_pointer);
//    printf("%s\n", user_information);

    cJSON *information_as_json = cJSON_Parse(user_information);
    cJSON *password_as_json = cJSON_GetObjectItem(information_as_json, "password");
    if (strcmp(cJSON_GetStringValue(password_as_json), password) != 0) {
        printf("%s | The password you entered is incorrect.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"Incorrect password.\"}");
        return 0;

    }
    generateToken();
    char message[1000] = "{\"type\":\"Token\",\"message\":\"";
    strcat(message, Token);
    strcat(message, "\"}");
    sendData(message);
    printf("%s | User %s successfully logged in.\n", getTime(), username);
    fclose(file_pointer);
    strcpy(USERNAME, username);
    return 1;
}

int getTweets(char *data) {
    char token[33];
    sscanf(data, "%*s %*s %s", token);
    data = strchr(data, ',') + 2;
    data[strlen(data) - 1] = '\0';
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    char filename[200];
    int id = 1;

    for (;; id++) {
        char str[5] = {0};
        strcpy(filename, "Resources/Tweets/");
        sprintf(str, "%d", id);
        strcat(filename, str);
        strcat(filename, ".tweet.json");
        if (fopen(filename, "r") == NULL) {
            break;
        }

    }

    file_pointer = fopen(filename, "w");
    cJSON *tweet_as_json, *comments;
    tweet_as_json = cJSON_CreateObject();
    comments = cJSON_CreateObject();
    cJSON_AddItemToObject(tweet_as_json, "id", cJSON_CreateNumber(id));
    cJSON_AddItemToObject(tweet_as_json, "author", cJSON_CreateString(USERNAME));
    cJSON_AddItemToObject(tweet_as_json, "content", cJSON_CreateString(data));
    cJSON_AddItemToObject(tweet_as_json, "comments", comments);
    cJSON_AddItemToObject(tweet_as_json, "likes", cJSON_CreateNumber(0));
    cJSON_AddArrayToObject(tweet_as_json, "seenBy");

    char *out = cJSON_PrintUnformatted(tweet_as_json);

    fprintf(file_pointer, "%s", out);
    fclose(file_pointer);
    char user_filename[100] = "Resources/Users/";
    strcat(user_filename, USERNAME);
    strcat(user_filename, ".user.json");
    file_pointer = fopen(user_filename, "r");
    char contents[MAX];
    fgets(contents, MAX, file_pointer);
    cJSON *user, *personal_tweets;
    user = cJSON_Parse(contents);
    personal_tweets = cJSON_GetObjectItem(user, "personalTweets");
    cJSON_AddItemToArray(personal_tweets, cJSON_CreateNumber(id));
    fclose(file_pointer);
    fopen(user_filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(user));
    fclose(file_pointer);
    printf("%s | User %s successfully send a tweet.\n", getTime(), USERNAME);
    sendData("{\"type\":\"Successful\",\"message\":\"Tweet is sent successfully.\"}");
    return 1;
}

int logout(char *data) {
    char token[33];
    sscanf(data, "%*s %s", token);
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    memset(Token, 0, strlen(Token) * sizeof(char));
    printf("%s | User %s successfully logged out.\n", getTime(), USERNAME);
    sendData("{\"type\":\"Successful\",\"message\":\"\"}");
    memset(USERNAME, 0, strlen(USERNAME) * sizeof(char));
    memset(current_file_name, 0, strlen(current_file_name) * sizeof(char));
    return 1;
}

char *getTime() {
    time_t current_time;
    time(&current_time);
    char *display = ctime(&current_time);
    display[strlen(display) - 1] = '\0';
    return display;
}

int refresh(char *data) {
    char token[33];
    sscanf(data, "%*s %s", token);
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer = fopen(current_file_name, "r");
    char user_file_content[MAX];
    fgets(user_file_content, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *user, *followings, *seen_by;
    user = cJSON_Parse(user_file_content);
    followings = cJSON_GetObjectItem(user, "followings");
    int n_followings = cJSON_GetArraySize(followings);
    strcpy(user_file_content, "");
    char *users_to_find[n_followings];
    int j = 0;
    int tweet_id_to_find[MAX];

    for (int i = 0; i < n_followings; ++i) {
        char user_filename[100] = "Resources/Users/", file_content[MAX];
        users_to_find[i] = cJSON_GetStringValue(cJSON_GetArrayItem(followings, i));
        strcat(user_filename, users_to_find[i]);
        strcat(user_filename, ".user.json");
        file_pointer = fopen(user_filename, "r");
        fgets(file_content, MAX, file_pointer);
        fclose(file_pointer);

        cJSON *tweets;
        tweets = cJSON_Parse(file_content);
        tweets = cJSON_GetObjectItem(tweets, "personalTweets");
        int n_tweet = cJSON_GetArraySize(tweets);
        for (int last_index = 0; last_index < n_tweet; ++j) {
            tweet_id_to_find[j] = (int) cJSON_GetNumberValue(cJSON_GetArrayItem(tweets, last_index));
            last_index++;
        }
    }
    sortInDescendingOrder(tweet_id_to_find, j);
    cJSON *message, *list;
    list = cJSON_CreateObject();
    cJSON_AddItemToObject(list, "type", cJSON_CreateString("List"));
    message = cJSON_AddArrayToObject(list, "message");


    for (int i = 0; i < j; ++i) {

        char tweet_filename[100] = "Resources/Tweets/", file_content[MAX], str[5];
        sprintf(str, "%d", tweet_id_to_find[i]);
        strcat(tweet_filename, str);
        strcat(tweet_filename, ".tweet.json");
        file_pointer = fopen(tweet_filename, "r");
        fgets(file_content, MAX, file_pointer);
        fclose(file_pointer);
        cJSON *tweet;
        tweet = cJSON_Parse(file_content);
        seen_by = cJSON_GetObjectItem(tweet, "seenBy");
        int dont = 0;
        for (int k = 0; k < cJSON_GetArraySize(seen_by); ++k) {
            if (!strcmp(USERNAME, cJSON_GetStringValue(cJSON_GetArrayItem(seen_by, k)))) {
                dont = 1;
            }
        }
        if (dont)
            continue;
        cJSON_AddItemToArray(seen_by, cJSON_CreateString(USERNAME));
        file_pointer = fopen(tweet_filename, "w");
        fprintf(file_pointer, "%s", cJSON_PrintUnformatted(tweet));
        fclose(file_pointer);
        cJSON_AddItemToArray(message, tweet);

    }
    char *out;
    out = cJSON_PrintUnformatted(list);
    printf("%s | User %s successfully refreshed.\n", getTime(), USERNAME);
    sendData(out);


    return 1;
}

int like(char *data) {
    char token[33];
    int tweet_id;
    sscanf(data, "%*s %s %d", token, &tweet_id);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    char tweet_filename[100] = "Resources/Tweets/", file_content[MAX], str[5];
    sprintf(str, "%d", tweet_id);
    strcat(tweet_filename, str);
    strcat(tweet_filename, ".tweet.json");
    FILE *file_pointer;
    file_pointer = fopen(tweet_filename, "r");
    if (file_pointer == NULL)
    {
        printf("%s | User %s can't like tweet %d\n", getTime(), USERNAME, tweet_id);
        sendData("{\"type\":\"Error\",\"message\":\"Tweet does not exist.\"}");
        return 0;
    }
    fgets(file_content, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *tweet, *author, *followers;
    int like;
    tweet = cJSON_Parse(file_content);
    author = cJSON_GetObjectItem(tweet, "author");
    char author_name[50];
    sprintf(author_name, "Resources/Users/%s.user.json", cJSON_GetStringValue(author));
    FILE *user_file_pointer;
    user_file_pointer = fopen(author_name, "r");
    char user_file_content[MAX];
    fgets(user_file_content, MAX, user_file_pointer);
    fclose(user_file_pointer);
    followers = cJSON_GetObjectItem(cJSON_Parse(user_file_content), "followers");
    int condition = 0;
    for (int i = 0; i < cJSON_GetArraySize(followers); ++i) {
        if (!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(followers, i)), USERNAME))
        {
            condition = 1;
        }
    }
    if (condition == 0)
    {
        printf("%s | User %s can't like tweet %d\n", getTime(), USERNAME, tweet_id);
        sendData("{\"type\":\"Error\",\"message\":\"Inaccessible tweet id.\"}");
        return 0;

    }
    like = (int) cJSON_GetNumberValue(cJSON_GetObjectItem(tweet, "likes"));
    like++;
    cJSON_DeleteItemFromObject(tweet, "likes");
    cJSON_AddItemToObject(tweet, "likes", cJSON_CreateNumber(like));
    file_pointer = fopen(tweet_filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(tweet));
    fclose(file_pointer);
    printf("%s | User %s successfully liked tweet %d\n", getTime(), USERNAME, tweet_id);
    char message[150];
    sprintf(message, "Response sent: {\"type\":\"Successful\",\"message\":\"Tweet %d successfully liked.\"}", tweet_id);
    sendData(message);
    return 1;

}

int comment(char *data) {
    char token[33];
    int tweet_id;
    sscanf(data, "%*s %s %d", token, &tweet_id);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    data = strchr(data, ',') + 2;
    data = strchr(data, ',') + 2;
//    data[strlen(data)-1] = '\0';

    char tweet_filename[100] = "Resources/Tweets/", file_content[MAX], str[5];
    sprintf(str, "%d", tweet_id);
    strcat(tweet_filename, str);
    strcat(tweet_filename, ".tweet.json");
    FILE *file_pointer;
    file_pointer = fopen(tweet_filename, "r");
    fgets(file_content, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *tweet, *comments;
    tweet = cJSON_Parse(file_content);
    comments = cJSON_GetObjectItem(tweet, "comments");
    cJSON_AddItemToObject(comments, USERNAME, cJSON_CreateString(data));
    file_pointer = fopen(tweet_filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(tweet));
    fclose(file_pointer);
    printf("%s | User %s successfully commented for tweet %d\n", getTime(), USERNAME, tweet_id);
    sendData("{\"type\":\"Successful\",\"message\":\"Comment is sent successfully.\"}");

    return 1;


}

int search(char *data) {
    char token[33], username[32];
    sscanf(data, "%*s %s %s", token, username);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    char filename[200] = "Resources/Users/";
    strcat(filename, username);
    strcat(filename, ".user.json");
    file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        printf("%s | This username does not exist.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"This username is not valid.\"}");
        return 0;
    }
    char user_information[MAX] = {0};
    fgets(user_information, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *user, *tweets, *user_in_user_file, *bio, *followers, *followings;
    user = cJSON_Parse(user_information);
    tweets = cJSON_GetObjectItem(user, "personalTweets");
    user_in_user_file = cJSON_GetObjectItem(user, "username");
    bio = cJSON_GetObjectItem(user, "bio");
    followers = cJSON_GetObjectItem(user, "followers");
    followings = cJSON_GetObjectItem(user, "followings");
    int n_followings = cJSON_GetArraySize(followings);
    int n_followers = cJSON_GetArraySize(followers);
    int is_following = 0;
    for (int i = 0; i < n_followers; ++i) {
        if (!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(followers, i)), USERNAME)) {
            is_following = 1;
            break;
        }
    }
    int n_tweets = cJSON_GetArraySize(tweets);
    int tweet_id[n_tweets];
    for (int i = 0; i < n_tweets; ++i) {
        tweet_id[i] = (int) cJSON_GetNumberValue(cJSON_GetArrayItem(tweets, i));
    }
    cJSON *message, *profile, *allTweets;
    profile = cJSON_CreateObject();
    message = cJSON_CreateObject();
    cJSON_AddItemToObject(profile, "type", cJSON_CreateString("Profile"));
    cJSON_AddItemToObject(profile, "message", message);
    cJSON_AddItemToObject(message, "username", user_in_user_file);
    cJSON_AddItemToObject(message, "bio", bio);
    cJSON_AddItemToObject(message, "numberOfFollowers", cJSON_CreateNumber(n_followers));
    cJSON_AddItemToObject(message, "numberOfFollowings", cJSON_CreateNumber(n_followings));
    if (is_following) {
        cJSON_AddItemToObject(message, "followStatus", cJSON_CreateString("Followed"));
    } else {
        cJSON_AddItemToObject(message, "followStatus", cJSON_CreateString("NotFollowed"));
    }
    allTweets = cJSON_AddArrayToObject(message, "allTweets");
    for (int i = 0; i < n_tweets; ++i) {
        char tweet_filename[100] = "Resources/Tweets/", file_content[MAX], str[5];
        sprintf(str, "%d", tweet_id[i]);
        strcat(tweet_filename, str);
        strcat(tweet_filename, ".tweet.json");
        file_pointer = fopen(tweet_filename, "r");
        fgets(file_content, MAX, file_pointer);
        fclose(file_pointer);
        cJSON *tweet;
        tweet = cJSON_Parse(file_content);
        cJSON_AddItemToArray(allTweets, tweet);
    }
    char *out;
    out = cJSON_PrintUnformatted(profile);
    printf("%s | User %s successfully searched for %s.\n", getTime(), USERNAME, username);
    sendData(out);
    return 1;
}

int unfollow(char *data) {
    char token[33], username[32];
    sscanf(data, "%*s %s %s", token, username);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    char filename[200] = "Resources/Users/";
    strcat(filename, username);
    strcat(filename, ".user.json");
    file_pointer = fopen(filename, "r");
    char user_information[MAX] = {0};
    fgets(user_information, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *user, *followers;
    user = cJSON_Parse(user_information);
    followers = cJSON_GetObjectItem(user, "followers");
    for (int i = 0; i < cJSON_GetArraySize(followers); ++i) {
        if (!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(followers, i)), USERNAME)) {
            cJSON_DeleteItemFromArray(followers, i);
        }
    }
    file_pointer = fopen(filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(user));
    fclose(file_pointer);
    strcpy(filename, "Resources/Users/");
    strcat(filename, USERNAME);
    strcat(filename, ".user.json");
    file_pointer = fopen(filename, "r");
    strcpy(user_information, "");
    fgets(user_information, MAX, file_pointer);
    fclose(file_pointer);
    user = cJSON_Parse(user_information);
    cJSON *followings;
    followings = cJSON_GetObjectItem(user, "followings");
    for (int i = 0; i < cJSON_GetArraySize(followers); ++i) {
        if (!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(followings, i)), username)) {
            cJSON_DeleteItemFromArray(followings, i);
        }
    }
    file_pointer = fopen(filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(user));
    fclose(file_pointer);
    printf("%s | User %s successfully unfollowed %s.\n", getTime(), USERNAME, username);
    char message[1000] = "{\"type\":\"Successful\",\"message\":\"User ";
    strcat(message, username);
    strcat(message, " is unfollowed successfully.\"}");
    sendData(message);
    return 1;
}

int follow(char *data) {
    char token[33], username[32];
    sscanf(data, "%*s %s %s", token, username);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    char filename[200] = "Resources/Users/";
    strcat(filename, username);
    strcat(filename, ".user.json");
    file_pointer = fopen(filename, "r");
    char user_information[MAX] = {0};
    fgets(user_information, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *user, *followers;
    user = cJSON_Parse(user_information);
    followers = cJSON_GetObjectItem(user, "followers");
    cJSON_AddItemToArray(followers, cJSON_CreateString(USERNAME));
    file_pointer = fopen(filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(user));
    fclose(file_pointer);
    strcpy(filename, "Resources/Users/");
    strcat(filename, USERNAME);
    strcat(filename, ".user.json");
    file_pointer = fopen(filename, "r");
    strcpy(user_information, "");
    fgets(user_information, MAX, file_pointer);
    fclose(file_pointer);
    user = cJSON_Parse(user_information);
    cJSON *followings;
    followings = cJSON_GetObjectItem(user, "followings");
    cJSON_AddItemToArray(followings, cJSON_CreateString(username));
    file_pointer = fopen(filename, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(user));
    fclose(file_pointer);
    printf("%s | User %s successfully followed %s.\n", getTime(), USERNAME, username);
    char message[1000] = "{\"type\":\"Successful\",\"message\":\"User ";
    strcat(message, username);
    strcat(message, " is followed successfully.\"}");
    sendData(message);
    return 1;
}

int tweetProfile(char *data) {
    char token[33];
    sscanf(data, "%*s %s", token);
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer = fopen(current_file_name, "r");
    char user_file_content[MAX];
    fgets(user_file_content, MAX, file_pointer);
    fclose(file_pointer);
    cJSON *user, *followings, *followers, *bio, *allTweets, *message, *username_as_json;
    user = cJSON_Parse(user_file_content);
    username_as_json = cJSON_GetObjectItem(user, "username");
    followings = cJSON_GetObjectItem(user, "followings");
    followers = cJSON_GetObjectItem(user, "followers");
    bio = cJSON_GetObjectItem(user, "bio");
    allTweets = cJSON_GetObjectItem(user, "personalTweets");

    int n_followings = cJSON_GetArraySize(followings);
    int n_followers = cJSON_GetArraySize(followers);
    int n_tweets = cJSON_GetArraySize(allTweets);
    cJSON *list, *Tweets;
    list = cJSON_CreateObject();
    cJSON_AddItemToObject(list, "type", cJSON_CreateString("Profile"));
    message = cJSON_AddObjectToObject(list, "message");
    cJSON_AddItemToObject(message, "username", username_as_json);
    cJSON_AddItemToObject(message, "bio", bio);
    cJSON_AddItemToObject(message, "numberOfFollowers", cJSON_CreateNumber(n_followers));
    cJSON_AddItemToObject(message, "numberOfFollowings", cJSON_CreateNumber(n_followings));
    cJSON_AddItemToObject(message, "followStatus", cJSON_CreateString("Yourself"));
    Tweets = cJSON_AddArrayToObject(message, "allTweets");

    for (int i = 0; i < n_tweets; ++i) {
        char tweet_filename[100] = "Resources/Tweets/", file_content[MAX], str[5];
        sprintf(str, "%d", (int) cJSON_GetNumberValue(cJSON_GetArrayItem(allTweets, i)));
        strcat(tweet_filename, str);
        strcat(tweet_filename, ".tweet.json");
        file_pointer = fopen(tweet_filename, "r");
        fgets(file_content, MAX, file_pointer);
        fclose(file_pointer);
        cJSON *tweet;
        tweet = cJSON_Parse(file_content);

        cJSON_AddItemToArray(Tweets, tweet);
    }
    char *out;
    out = cJSON_PrintUnformatted(list);
    printf("%s | User %s successfully viewed his/her profile.\n", getTime(), USERNAME);
    sendData(out);
    return 1;

}

int setBio(char *data) {
    char token[33];
    sscanf(data, "%*s %*s %s", token);
    data = strchr(data, ',') + 2;
    data[strlen(data) - 1] = '\0';
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    file_pointer = fopen(current_file_name, "r");
    fclose(file_pointer);
    char file_content[MAX];
    fgets(file_content, MAX, file_pointer);
    cJSON *information;
    information = cJSON_Parse(file_content);
    cJSON_ReplaceItemInObject(information, "bio", cJSON_CreateString(data));
    file_pointer = fopen(current_file_name, "w");
    fprintf(file_pointer, "%s", cJSON_PrintUnformatted(information));
    fclose(file_pointer);

    printf("%s | User %s successfully changed his/her bio.\n", getTime(), USERNAME);
    sendData("{\"type\":\"Successful\",\"message\":\"Bio is updated successfully.\"}");
    return 1;
}

int changePassword(char *data) {
    char token[33], current_password[15], new_password[15];
    sscanf(data, "%*s %*s %s %s %s", token, current_password, new_password);
    token[strlen(token) - 1] = '\0';
    current_password[strlen(current_password) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    if (!strcmp(current_password, new_password)) {
        printf("%s | The new password was the same as old one.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"New password is equal to current password.\"}");
        return 0;
    }
    FILE *file_pointer;
    file_pointer = fopen(current_file_name, "r");
    char file_content[MAX];
    fgets(file_content, MAX, file_pointer);
    cJSON *information;
    information = cJSON_Parse(file_content);
    fclose(file_pointer);
    FILE *filePointer;
    char *old_password;
    old_password = cJSON_GetStringValue(cJSON_GetObjectItem(information, "password"));
    if (strcmp(current_password, old_password) != 0) {
        printf("%s | Current password did not match.\n", getTime());
        sendData("{\"type\":\"Error\",\"message\":\"Entered current password is wrong.\"}");
        return 0;
    }
    cJSON_ReplaceItemInObject(information, "password", cJSON_CreateString(new_password));
    filePointer = fopen(current_file_name, "w");
    fprintf(filePointer, "%s", cJSON_PrintUnformatted(information));
    fclose(filePointer);
    printf("%s | User %s successfully changed his/her password.\n", getTime(), USERNAME);
    sendData("{\"type\":\"Successful\",\"message\":\"Password is changed successfully.\"}");
    return 1;
}

void sortInDescendingOrder(int number[], int size) {
    int i, j, a;
    for (i = 0; i < size; ++i) {
        for (j = i + 1; j < size; ++j) {
            if (number[i] < number[j]) {
                a = number[i];
                number[i] = number[j];
                number[j] = a;
            }
        }
    }
}

int deleteTweet(char *data) {
    char token[33];
    int id;
    sscanf(data, "%*s %s %d", token, &id);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    FILE *file_pointer;
    file_pointer = fopen(current_file_name, "r");
    char file_content[MAX];
    fgets(file_content, MAX, file_pointer);
    cJSON *information, *personalTweets;
    information = cJSON_Parse(file_content);
    personalTweets = cJSON_GetObjectItem(information, "personalTweets");
    fclose(file_pointer);
    int is_successful = 0;
    for (int i = 0; i < cJSON_GetArraySize(personalTweets); ++i) {
        if (cJSON_GetNumberValue(cJSON_GetArrayItem(personalTweets, i)) == id) {
            cJSON_DeleteItemFromArray(personalTweets, i);
            is_successful = 1;
        }

    }
    if (is_successful == 0) {
        printf("%s | User %s could not delete the tweet with id %d.\n", getTime(), USERNAME, id);
        sendData("{\"type\":\"Error\",\"message\":\"This tweet does not exist or is not yours.\"}");
        return 0;
    }
    char tweet_filename[100];
    sprintf(tweet_filename, "Resources/Tweets/%d.tweet.json", id);
    remove(tweet_filename);
    FILE *filePointer;
    filePointer = fopen(current_file_name, "w");
    fprintf(filePointer, "%s", cJSON_PrintUnformatted(information));
    fclose(filePointer);
    printf("%s | User %s successfully deleted tweet %d.\n", getTime(), USERNAME, id);
    char message[150];
    sprintf(message, "{\"type\":\"Successful\",\"message\":\"Tweet %d is deleted successfully.\"}", id);
    sendData(message);
    return 1;
    //you should delete the tweet file too!
}

int fuzzySearch(char *data) {
    char token[33], username[32];
    sscanf(data, "%*s %s %s", token, username);
    token[strlen(token) - 1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    const char dir_name[] = "Resources/Users";
    struct dirent *de;

    DIR *dr = opendir(dir_name);

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }

    int a = 0, k = 0;
    char *user_files[1000];
    while ((de = readdir(dr)) != NULL) {
        if (a > 1) {
            user_files[k] = strtok(de->d_name, ".");
            k++;
        }
        a++;
    }
    closedir(dr);
    cJSON *message, *users;
    message = cJSON_CreateObject();
    users = cJSON_CreateArray();
    cJSON_AddItemToObject(message, "users", users);
    for (int j = 0; j < k; ++j) {
        if (isSubstring(username, user_files[j])) {
            if (strcmp(user_files[j], USERNAME))
                cJSON_AddItemToArray(users, cJSON_CreateString(user_files[j]));

        }
    }
    printf("%s | User %s successfully fuzzy searched %s.\n", getTime(), USERNAME, username);
    sendData(cJSON_PrintUnformatted(message));
    return 1;
}

int isSubstring(char s1[], char *s2) {
    int M = strlen(s1);
    int N = strlen(s2);
    for (int i = 0; i <= N - M; i++) {
        int j;
        for (j = 0; j < M; j++)
            if (s2[i + j] != s1[j])
                break;
        if (j == M)
            return 1;
    }
    return 0;
}

int tweetSearch(char *data)
{
    char token[33], hashtag[32];
    sscanf(data, "%*s %s %s", token, hashtag);
    token[strlen(token) - 1] = '\0';
    hashtag[strlen((hashtag))-1] = '\0';
    if (strcmp(token, Token) != 0) {
        printf("%s | User not registered.\n", getTime());
        return 0;
    }
    char tweet_file_name[50] = "Resources/Tweets/";
    struct dirent *de;
    char hashtag_to_search[50] = "#";
    strcat(hashtag_to_search, hashtag);
    DIR *dr = opendir(tweet_file_name);

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }
    cJSON *message, *tweets_with_hashtag;
    message = cJSON_CreateObject();
    cJSON_AddItemToObject(message, "type", cJSON_CreateString("Tweets"));
    tweets_with_hashtag = cJSON_CreateArray();
    cJSON_AddItemToObject(message, "allTweets", tweets_with_hashtag);

    int a = 0, k = 0;
    char tweet_file_content[MAX];
    while ((de = readdir(dr)) != NULL) {
        if (strlen(de->d_name) > 3){
            FILE *file_pointer;
            char file_name[50] = "Resources/Tweets/";
            strcat(file_name, de->d_name);
            file_pointer = fopen(file_name, "r");
            fgets(tweet_file_content, MAX, file_pointer);
            fclose(file_pointer);
            cJSON *tweet;
            tweet = cJSON_GetObjectItem(cJSON_Parse(tweet_file_content), "content");
            if (isSubstring(hashtag_to_search, cJSON_PrintUnformatted(tweet)))
            {
                cJSON_AddItemToArray(tweets_with_hashtag, cJSON_Parse(tweet_file_content));
            }

        }
    }
    closedir(dr);
    printf("%s | User %s successfully searched tweets for %s.\n", getTime(), USERNAME, hashtag_to_search);
    sendData(cJSON_PrintUnformatted(message));
    return 1;
}