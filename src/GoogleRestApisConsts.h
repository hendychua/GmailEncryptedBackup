#ifndef GOOGLE_APIS_CONSTS_H
#define GOOGLE_APIS_CONSTS_H

#include <string>

/// <summary> Define all constants used by Google API in this file. </summary>

// Host for getting auth code, access token and refresh token
static const std::string GOOGLE_OAUTH_HOST = "https://accounts.google.com";
// Host for using google apis
static const std::string GOOGLE_APIS_HOST = "https://www.googleapis.com";

static const std::string AUTH_CODE_ENDPOINT = "/o/oauth2/v2/auth";
static const std::string ACCESS_TOKEN_ENDPOINT = "/o/oauth2/token";

// me is a keyword that refers to the user who granted the authorization_code
static const std::string LIST_MESSAGES_ENDPOINT = "/gmail/v1/users/me/messages";
static const std::string GET_MESSAGE_DETAILS_ENDPOINT = "/gmail/v1/users/me/messages";

#endif
