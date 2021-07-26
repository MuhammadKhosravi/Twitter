#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <json-c/json.h>
#include "cJSON.h"

#define PORT 12345
#define SA struct sockaddr
#define jo struct json_object
#define MAX 1000

char *sendData(char *data);

void personalArea();

void like(int);

void comment(int);

void setbio();

void changePassword();

void logout();

int initialize(int port);

void refresh();

void search();

void sendTweet();

void tweetProfile();

int getch(void);

void timeline();

void signup();

void menu();

void login();

int isSubstring(char s1[], char s2[]);

int countComments(char *);

void startMenu();

void deleteTweet();

void fuzzySearch();

void tweetSearch();


struct sockaddr_in servaddr;
char token[30];

int main() {
    int port = 12345;
    int can_initialize = initialize(port);
    startMenu();
}

void startMenu() {
    system("clear");
    printf("Hello and welcome to Twitter!"
           "\nHave an account? Type 1 and hit enter to log in!"
           "\nNew to Twitter? Type 2 and hit enter to sign up!\n");
    int num;
    scanf("%d", &num);
    switch (num) {
        case 1:
            system("clear");
            login();
            break;
        case 2:
            system("clear");
            signup();
            sleep(2);
            system("clear");
            break;
        default:
            system("clear");
            printf("Please send either 1 or 2.\n");
            sleep(2);
            system("clear");
            startMenu();
            break;
    }
}

int initialize(int port) {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    return 0;
}

char *sendData(char *data) {
    char *buffer = malloc(2000);
    memset(buffer, 0, 2000);
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(client_socket, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        return buffer;
    }
    send(client_socket, data, strlen(data), 0);
    recv(client_socket, buffer, 999, 0);
    close(client_socket);
    return buffer;
}

int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void signup() {
    char message[50] = "signup ";
    char username[15];
    char password[20];
    printf("Enter your username and hit enter\n");
    scanf("%*c%[^\n]s", username);
    printf("Enter your password and hit enter\n");
    getch();
    int p = 0;
    do {
        password[p] = getch();
        if (password[p] != '\n') {
            printf("*");
        }
        p++;
    } while (password[p - 1] != '\n');
    password[p - 1] = '\0';
    printf("\n");
    strcat(message, username);
    strcat(message, ", ");
    strcat(message, password);
    strcat(message, "\n");
    char *feedback = sendData(message);
    // printf("%s\n", feedback);/////////////////////////////
    // getchar();
    if (isSubstring("Successful", feedback)) {
        system("clear");
        printf("Signed up successfully!\n");
        sleep(2);
        system("clear");
        strcpy(message, "login ");
        strcat(message, username);
        strcat(message, ", ");
        strcat(message, password);
        strcat(message, "\n");
        feedback = sendData(message);
        strncpy(token, feedback + 27, 59 - 27);
        menu();
    } else {
        printf("Username already exists. please try another one.\n");
        sleep(2);
        system("clear");
        signup();
    }
}

void login() {
    char message[50] = "login ";
    char username[15];
    char password[20];
    printf("Enter your username and hit enter\n");
    scanf("%*c%[^\n]s", username);
    printf("Enter your password and hit enter\n");
    getch();
    int p = 0;
    do {
        password[p] = getch();
        if (password[p] != '\n') {
            printf("*");
        }
        p++;
    } while (password[p - 1] != '\n');
    password[p - 1] = '\0';
    printf("\n");
    strcat(message, username);
    strcat(message, ", ");
    strcat(message, password);
    strcat(message, "\n");
    char *feedback = sendData(message);
    if (isSubstring("Token", feedback)) {
        strncpy(token, feedback + 27, 59 - 27);
        printf("You are successfully logged in!\n");
        sleep(2);
        system("clear");
        menu();
    } else {
        if (isSubstring("username", feedback)) {
            printf("This username is not valid.\n");
        } else if (isSubstring("password", feedback)) {
            printf("Incorrect password. Please try again.\n");
        } else if (isSubstring("already", feedback)) {
            printf("User already logged in!\n");
        }
        sleep(2);
        system("clear");
        startMenu();
    }
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

void menu() {
    fflush(stdin);
    int option;
    do {
        system("clear");
        printf("Welcome!"
               "\n1. Timeline"
               "\n2. Search"
               "\n3. Tweet Profile"
               "\n4. Personal Area"
               "\n5. Log Out\n");
        scanf("%d", &option);
    } while (option > 5);
    system("clear");
    switch (option) {
        case 1:
            timeline();
            getchar();
            break;
        case 2:
            search();
            break;
        case 3:
            tweetProfile();
            break;
        case 4:
            personalArea();
            break;
        case 5:
            logout();
            break;
    }
}

void timeline() {
    int option;
    do {
        printf("1. Send Tweet"
               "\n2. Refresh"
               "\n3. Back\n");
        scanf("%d", &option);
    } while (option > 3);
    system("clear");
    fflush(stdin);
    getchar();
    switch (option) {
        case 1:
            sendTweet();
            break;
        case 2:
            refresh();
            break;
        case 3:
            menu();
            break;
    }
}

void sendTweet() {
    printf("Enter your tweet here:(maximum characters: 1000)\n");
    char tweet[MAX];
    fgets(tweet, MAX, stdin);
    char message[1100] = "send tweet ";
    strcat(message, token);
    strcat(message, ", ");
    strcat(message, tweet);
    strcat(message, "\n");
    sendData(message);
    system("clear");
    timeline();
}

void refresh() {
    char *feedback = malloc(2000);
    memset(feedback, 1999, 0);
    char data[40] = "refresh ";
    strcat(data, token);
    strcat(data, "\n");
    feedback = sendData(data);
    jo *parsed_json, *messages, *message = malloc(1000);
    memset(message, 999, 0);
    parsed_json = json_tokener_parse(feedback);
    json_object_object_get_ex(parsed_json, "message", &messages);
    memcpy(message, messages, 1000);
    size_t n_tweets = json_object_array_length(message);
    int comment_order[n_tweets];
    for (int j = 0; j < n_tweets; ++j) {
        jo *tweet = json_object_array_get_idx(message, j);
        jo *id, *author, *content, *comments, *likes;
        json_object_object_get_ex(tweet, "id", &id);
        json_object_object_get_ex(tweet, "author", &author);
        json_object_object_get_ex(tweet, "content", &content);
        json_object_object_get_ex(tweet, "comments", &comments);
        json_object_object_get_ex(tweet, "likes", &likes);
        const char *tweet_author = json_object_get_string(author);
        const char *tweet_content = json_object_get_string(content);
        int tweet_id = json_object_get_int(id);
        comment_order[j] = tweet_id;
        int n_comments = countComments(json_object_get_string(comments));
        int tweet_likes = json_object_get_int(likes);
        printf("%s      %d\n%s\nLikes: %d     comments: %d\n------------------------\n",
               tweet_author, tweet_id, tweet_content, tweet_likes, n_comments);
    }

    printf("0. Back to menu"
           "\n1. Like"
           "\n2. Comment"
           "\n3. Show comments\n");
    int option;
    scanf("%d", &option);
    getchar();
    system("clear");
    int ID;
    switch (option) {
        case 0:
            menu();
            break;
        case 1:
            printf("Enter the *id* of the tweet you want to like:\n");
            scanf("%d", &ID);
            like(ID);
            timeline();
            break;
        case 2:
            printf("Enter the *id* of the tweet you want to comment:\n");
            scanf("%d", &ID);
            // getchar();
            comment(ID);
            timeline();
            break;
        case 3:
            printf("Enter the *id* of the tweet you want to show its comments:\n");
            scanf("%d", &ID);
            int real_id;
            for (int k = 0; k < n_tweets; k++) {
                if (comment_order[k] == ID) {
                    real_id = k;
                    break;
                }
            }

            jo *tweet = json_object_array_get_idx(message, real_id), *comments;
            json_object_object_get_ex(tweet, "comments", &comments);
            const char *comment_str = json_object_get_string(comments);
            int counter = 0;
            char c = '\"';
            for (int i = 0; i < strlen(comment_str); ++i) {
                if (comment_str[i] == c) {
                    i++;
                    for (; i < strlen(comment_str); i++) {
                        if (comment_str[i] == c) {
                            break;
                        }
                        printf("%c", comment_str[i]);
                    }
                    if (counter % 2) {
                        printf("\n");
                    } else {
                        printf(": ");
                    }
                    counter++;
                }
            }
            printf("\n0. Back to menu\n");
            int op;
            do {
                scanf("%d", &op);
            } while (op > 0 && printf("Try again.\n"));
            free(feedback);
            free(message);
            system("clear");
            if (op == 0)
                menu();
            break;
    }
}

void logout() {
    char message[50] = "logout ";
    strcat(message, token);
    strcat(message, "\n");
    sendData(message);
    printf("You are successfully logged off!\n");
    system("clear");
    startMenu();
}

void personalArea() {
    fflush(stdin);
    int option;
    do {
        printf("1. Set bio"
               "\n2. Change password"
               "\n3. Back\n");
        scanf("%d", &option);
    } while (option >= 4);
    getchar();
    system("clear");
    switch (option) {
        case 1:
            setbio();
            break;
        case 2:
            changePassword();
            break;
        case 3:
            menu();
            break;
    }
}

void setbio() {
    printf("Enter your bio: (maximum 80 characters)\n");
    char message[150] = "set bio ";
    strcat(message, token);
    strcat(message, ", ");
    char bio[80];
    fgets(bio, 80, stdin);
    strcat(message, bio);
    strcat(message, "\n");
    char *feedback = sendData(message);
    system("clear");
    if (isSubstring("successfully", feedback)) {
        printf("Your bio was successfully set!\n");
    } else {
        printf("Your can't change your bio twice!\n");
    }
    sleep(2);
    system("clear");
    personalArea();
}

void changePassword() {
    char data[200] = "change password ";
    strcat(data, token);
    strcat(data, ", ");
    printf("Enter your current password:\n");
    char current_password[20], new_password[20];
    scanf("%s", current_password);
    printf("Enter your new password:\n");
    scanf("%s", new_password);
    strcat(data, current_password);
    strcat(data, ", ");
    strcat(data, new_password);
    strcat(data, "\n");
    char *feedback = sendData(data);
    if (isSubstring("successfully", feedback)) {
        printf("Password changed successfully!\n");
    } else {
        printf("Password did not change! You must have misspelled your password.\n");
    }
    sleep(2);
    system("clear");
    menu();
}

void tweetProfile() {
    char data[80] = "profile ";
    strcat(data, token);
    strcat(data, "\n");
    char *buffer = sendData(data);

    jo *parsed_json, *type, *messages = malloc(10000), *message = malloc(10000);
    size_t n_messages;
    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "message", &messages);

    memcpy(message, messages, 10000);

    jo *username, *bio, *numberOfFollowers, *numberOfFollowings, *followStatus, *allTweets;
    json_object_object_get_ex(message, "username", &username);
    json_object_object_get_ex(message, "bio", &bio);
    json_object_object_get_ex(message, "numberOfFollowers", &numberOfFollowers);
    json_object_object_get_ex(message, "numberOfFollowings", &numberOfFollowings);
    json_object_object_get_ex(message, "followStatus", &followStatus);
    json_object_object_get_ex(message, "allTweets", &allTweets);
    jo *shit = json_tokener_parse(json_object_get_string(allTweets));
    n_messages = json_object_array_length(shit);
    getchar();
    char *user_name = json_object_get_string(username);
    char *user_bio = json_object_get_string(bio);
    int n_followers = json_object_get_int(numberOfFollowers);
    int n_followings = json_object_get_int(numberOfFollowings);
    printf("Username: %s\n"
           "Bio: %s\n"
           "Followers: %d    Followings: %d\n"
           "\n", user_name, user_bio, n_followers, n_followings);

    jo *id, *author, *content, *comments, *likes;
    for (int j = 0; j < n_messages; ++j) {
        jo *tweet = json_object_array_get_idx(shit, j);
        json_object_object_get_ex(tweet, "id", &id);
        json_object_object_get_ex(tweet, "author", &author);
        json_object_object_get_ex(tweet, "content", &content);
        json_object_object_get_ex(tweet, "comments", &comments);
        json_object_object_get_ex(tweet, "likes", &likes);

        const char *tweet_content = json_object_get_string(content);
        int n_comments = countComments(json_object_get_string(comments));
        int tweet_likes = json_object_get_int(likes);
        printf("tweet id: %d\n%s\nLikes: %d     comments: %d\n------------------------\n",
               json_object_get_int(id), tweet_content, tweet_likes, n_comments);
    }
    printf("0. Back to menu"
           "\n1. Delete a tweet\n");
    int option;
    scanf("%d", &option);
    getchar();
    system("clear");
    if (option == 0)
        menu();
    else if (option == 1)
        deleteTweet();
    free(messages);
    free(message);
}

void search() {
    int option;
    do {
        system("clear");
        printf("1. Normal search"
               "\n2. Fuzzy search"
               "\n3. Tweet search"
               "\n4. Back to menu\n");
        scanf("%d", &option);
    } while (option > 4);
    system("clear");
    if (option == 2) {
        fuzzySearch();
        return;
    }
    if (option == 3){
        tweetSearch();
        return;
    }
    if (option == 4){
        menu();
        return;
    }
    printf("Type the username you are looking for:\n");
    char data[100] = "search ";
    strcat(data, token);
    strcat(data, ", ");
    char usrname[20];
    scanf("%s", usrname);
    strcat(data, usrname);
    strcat(data, "\n");
    const char *feedback = sendData(data);
    if (isSubstring("Profile", feedback)) {
        jo *parsed_json, *type, *messages, *message;

        size_t n_messages;

        parsed_json = json_tokener_parse(feedback);
        json_object_object_get_ex(parsed_json, "message", &messages);

        message = json_tokener_parse(json_object_get_string(messages));

        jo *username, *bio, *numberOfFollowers, *numberOfFollowings, *followStatus, *allTweets;
        json_object_object_get_ex(message, "username", &username);
        json_object_object_get_ex(message, "bio", &bio);
        json_object_object_get_ex(message, "numberOfFollowers", &numberOfFollowers);
        json_object_object_get_ex(message, "numberOfFollowings", &numberOfFollowings);
        json_object_object_get_ex(message, "followStatus", &followStatus);
        json_object_object_get_ex(message, "allTweets", &allTweets);
        jo *shit = json_tokener_parse(json_object_get_string(allTweets));
        n_messages = json_object_array_length(shit);
        getchar();
        char *user_name = json_object_get_string(username);
        char *user_bio = json_object_get_string(bio);
        int n_followers = json_object_get_int(numberOfFollowers);
        int n_followings = json_object_get_int(numberOfFollowings);
        char follow_status[10];
        if (!strcmp(json_object_get_string(followStatus), "NotFollowed")) {
            strcpy(follow_status, "Not followed");
        } else {
            strcpy(follow_status, "Followed");

        }
        printf("Username: %s"
               "\nBio: %s"
               "\nFollowers: %d    Followings: %d"
               "\nFollow status: %s\n\n", user_name, user_bio, n_followers, n_followings, follow_status);
        jo *id, *author, *content, *comments, *likes;
        for (int j = 0; j < n_messages; ++j) {
            jo *tweet = json_object_array_get_idx(shit, j);
            json_object_object_get_ex(tweet, "id", &id);
            json_object_object_get_ex(tweet, "author", &author);
            json_object_object_get_ex(tweet, "content", &content);
            json_object_object_get_ex(tweet, "comments", &comments);
            json_object_object_get_ex(tweet, "likes", &likes);

            const char *tweet_content = json_object_get_string(content);
            int n_comments = countComments(json_object_get_string(comments));
            int tweet_likes = json_object_get_int(likes);
            printf("%s\nLikes: %d     comments: %d\n------------------------\n",
                   tweet_content, tweet_likes, n_comments);
        }
        printf("0. Back to menu"
               "\n1. Follow/Unfollow\n");
        int option;
        scanf("%d", &option);
        getchar();
        system("clear");
        if (option == 0)
            menu();
        else if (option == 1) {
            if (!strcmp(json_object_get_string(followStatus), "NotFollowed")) {
                strcpy(data, "follow ");
                strcat(data, token);
                strcat(data, ", ");
                strcat(data, user_name);
                strcat(data, "\n");
                sendData(data);
            } else {
                strcpy(data, "unfollow ");
                strcat(data, token);
                strcat(data, ", ");
                strcat(data, user_name);
                strcat(data, "\n");
                sendData(data);
            }
            system("clear");
            printf("Action done successfully.\n");
            sleep(2);
            menu();
        }
    } else {
        system("clear");
        printf("No users found with that username you entered.\n");
        sleep(2);
        system("clear");
        menu();
    }
}

void like(int id) {
    char data[150] = "like ";
    strcat(data, token);
    strcat(data, ", ");
    char str_id[10];
    sprintf(str_id, "%d", id);
    strcat(data, str_id);
    strcat(data, "\n");
    const char *feedback =sendData(data);
    if (isSubstring("exist", feedback))
    {
        system("clear");
        printf("Tweet does not exit!\n");

    }
    else if (isSubstring("Inaccessible", feedback))
    {
        system("clear");
        printf("This tweet is not in your feed!\n");
    }
    else if (isSubstring("Successful", feedback))
    {
        system("clear");
        printf("Tweet successfully liked!\n");
    }
    sleep(2);
    system("clear");
    fflush(stdin);
}

void comment(int id) {
    getchar();
    char data[250] = "comment ";
    strcat(data, token);
    strcat(data, ", ");
    char str_id[10];
    sprintf(str_id, "%d", id);
    strcat(data, str_id);
    strcat(data, ", ");
    char message[50];
    printf("Type your comment here: (maximum 50 characters)\n");
    fgets(message, "%s", stdin);
    strcat(data, message);
    strcat(data, "\n");
    sendData(data);
    system("clear");
    printf("Comment successfully submitted!\n");
    sleep(2);
    system("clear");
    fflush(stdin);
}

int countComments(char *s) {
    char c = ':';
    int res = 0;

    for (int i = 0; i < strlen(s); i++)
        if (s[i] == c)
            res++;

    return res;
}

void deleteTweet() {
    printf("Enter the id of the tweet you want to delete:\n");
    int id;
    scanf("%d", &id);
    char data[50] = "delete ";
    strcat(data, token);
    strcat(data, ", ");
    char str_id[10];
    sprintf(str_id, "%d", id);
    strcat(data, str_id);
    strcat(data, "\n");
    char *feedback = sendData(data);
    system("clear");
    if (isSubstring("Successful", feedback))
        printf("Tweet successfully deleted!\n");
    else
        printf("This tweet does not exist or does not belong to you.\n");
    sleep(2);
    system("clear");
    fflush(stdin);
    tweetProfile();
}

void fuzzySearch() {
    printf("Type the username you are looking for (or a part of it's username):\n");
    char data[100] = "fsearch ";
    strcat(data, token);
    strcat(data, ", ");
    char usrname[20];
    scanf("%s", usrname);
    strcat(data, usrname);
    strcat(data, "\n");
    const char *feedback1 = sendData(data);
    jo *possible_users, *users;
    possible_users = json_tokener_parse(feedback1);
    json_object_object_get_ex(possible_users, "users", &users);
    int n_users = json_object_array_length(users);
    char *usernames[n_users];
    for (int i = 0; i < n_users; ++i) {
        jo *a_user = json_object_array_get_idx(users, i);
        usernames[i] = json_object_get_string(a_user);
        printf("%d: %s\n", i + 1, usernames[i]);
    }
    int select_user;
    do {
//        system("clear");
        scanf("%d", &select_user);
    } while (select_user > n_users);
    system("clear");
    select_user--;
    strcpy(data, "search ");
    strcat(data, token);
    strcat(data, ", ");
    strcat(data, usernames[select_user]);
    strcat(data, "\n");
    const char *feedback = sendData(data);
    //starting the usual search
    if (isSubstring("Profile", feedback)) {
        jo *parsed_json, *type, *messages, *message;

        size_t n_messages;

        parsed_json = json_tokener_parse(feedback);
        json_object_object_get_ex(parsed_json, "message", &messages);

        message = json_tokener_parse(json_object_get_string(messages));

        jo *username, *bio, *numberOfFollowers, *numberOfFollowings, *followStatus, *allTweets;
        json_object_object_get_ex(message, "username", &username);
        json_object_object_get_ex(message, "bio", &bio);
        json_object_object_get_ex(message, "numberOfFollowers", &numberOfFollowers);
        json_object_object_get_ex(message, "numberOfFollowings", &numberOfFollowings);
        json_object_object_get_ex(message, "followStatus", &followStatus);
        json_object_object_get_ex(message, "allTweets", &allTweets);
        jo *shit = json_tokener_parse(json_object_get_string(allTweets));
        n_messages = json_object_array_length(shit);
        getchar();
        char *user_name = json_object_get_string(username);
        char *user_bio = json_object_get_string(bio);
        int n_followers = json_object_get_int(numberOfFollowers);
        int n_followings = json_object_get_int(numberOfFollowings);
        char follow_status[10];
        if (!strcmp(json_object_get_string(followStatus), "NotFollowed")) {
            strcpy(follow_status, "Not followed");
        } else {
            strcpy(follow_status, "Followed");

        }
        printf("Username: %s"
               "\nBio: %s"
               "\nFollowers: %d    Followings: %d"
               "\nFollow status: %s\n\n", user_name, user_bio, n_followers, n_followings, follow_status);
        jo *id, *author, *content, *comments, *likes;
        for (int j = 0; j < n_messages; ++j) {
            jo *tweet = json_object_array_get_idx(shit, j);
            json_object_object_get_ex(tweet, "id", &id);
            json_object_object_get_ex(tweet, "author", &author);
            json_object_object_get_ex(tweet, "content", &content);
            json_object_object_get_ex(tweet, "comments", &comments);
            json_object_object_get_ex(tweet, "likes", &likes);

            const char *tweet_content = json_object_get_string(content);
            int n_comments = countComments(json_object_get_string(comments));
            int tweet_likes = json_object_get_int(likes);
            printf("%s\nLikes: %d     comments: %d\n------------------------\n",
                   tweet_content, tweet_likes, n_comments);
        }
        printf("0. Back to menu"
               "\n1. Follow/Unfollow\n");
        int option;
        scanf("%d", &option);
        getchar();
        system("clear");
        if (option == 0)
            menu();
        else if (option == 1) {
            if (!strcmp(json_object_get_string(followStatus), "NotFollowed")) {
                strcpy(data, "follow ");
                strcat(data, token);
                strcat(data, ", ");
                strcat(data, user_name);
                strcat(data, "\n");
                sendData(data);
            } else {
                strcpy(data, "unfollow ");
                strcat(data, token);
                strcat(data, ", ");
                strcat(data, user_name);
                strcat(data, "\n");
                sendData(data);
            }
            system("clear");
            printf("Action done successfully.\n");
            sleep(2);
            menu();
        }
    } else {
        system("clear");
        printf("No users found with that username you entered.\n");
        sleep(2);
        system("clear");
        menu();
    }
}

void tweetSearch()
{
    printf("Type the hashtag you are looking for:\n#");
    char data[100] = "tsearch ";
    strcat(data, token);
    strcat(data, ", ");
    char hashtag[20];
    scanf("%s", hashtag);
    strcat(data, hashtag);
    strcat(data, "\n");
    const char *feedback = sendData(data);
    jo *message;
    json_object_object_get_ex(json_tokener_parse(feedback), "allTweets", &message);
    size_t n_tweets = json_object_array_length(message);
    int comment_order[n_tweets];
    for (int j = 0; j < n_tweets; ++j) {
        jo *tweet = json_object_array_get_idx(message, j);
        jo *id, *author, *content, *comments, *likes;
        json_object_object_get_ex(tweet, "id", &id);
        json_object_object_get_ex(tweet, "author", &author);
        json_object_object_get_ex(tweet, "content", &content);
        json_object_object_get_ex(tweet, "comments", &comments);
        json_object_object_get_ex(tweet, "likes", &likes);
        const char *tweet_author = json_object_get_string(author);
        const char *tweet_content = json_object_get_string(content);
        int tweet_id = json_object_get_int(id);
        comment_order[j] = tweet_id;
        int n_comments = countComments(json_object_get_string(comments));
        int tweet_likes = json_object_get_int(likes);
        printf("%s      %d\n%s\nLikes: %d     comments: %d\n------------------------\n",
               tweet_author, tweet_id, tweet_content, tweet_likes, n_comments);
    }
    int option;
    do {
        printf("0. Back to menu\n");
        scanf("%d", &option);
    } while (option > 0);
    system("clear");
    if (option == 0) {
        menu();
        return;
    }

}