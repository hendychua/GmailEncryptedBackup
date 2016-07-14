#ifndef GOOGLE_AUTHORIZATION_CLIENT_H
#define GOOGLE_AUTHORIZATION_CLIENT_H

#include <vector>
#include <string>

/// <summary> A helper struct to store access token details together. </summary>
struct AccessTokenDetails
{
    std::string accessToken;
    std::string refreshToken;
    std::string tokenType;
    int expiresIn;
};

/// <summary> A wrapper around Google's API to get auth codes. </summary>
class GoogleAuthorizationClient
{
public:
    /// <summary> Constructor that takes in Google ClientId and ClientSecret </summary>
    GoogleAuthorizationClient(const std::string &_clientId, const std::string &_clientSecret);

    /// <summary> Gets authorization code from Google. Note that this is not the access token. </summary>
    std::string GetAuthorizationCode(const std::vector<std::string> &scopes,
        const std::string &mailbox, const std::string &redirectUri);

    /// <summary> Exchanges an auth code for access token. Returns AccessTokenDetails struct. </summary>
    AccessTokenDetails ExchangeForAccessToken(const std::string &authorizationCode, const std::string &redirectUri);

private:
    std::string clientId;
    std::string clientSecret;
};

#endif
