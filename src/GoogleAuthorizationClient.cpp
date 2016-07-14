#include "GoogleAuthorizationClient.h"
#include "GoogleRestApisConsts.h"

#include "cpprest/http_client.h"
#include "cpprest/json.h"

#include <iostream>
#include <stdexcept>

// cpprest
using namespace web;
using namespace web::http;
using namespace web::http::client;

// the to_utf8string() conversion method is needed because although
// casablanca's utility::string_t is typedef'd as std::string on linux platforms,
// it is typedef'd as wstring on windows.
// using the conversion method ensures that it will be cast to std::string on all platforms.
using utility::conversions::to_utf8string;

using namespace std;

GoogleAuthorizationClient::GoogleAuthorizationClient(const string &_clientId, const string &_clientSecret)
    : clientId(_clientId), clientSecret(_clientSecret)
{
}

string GoogleAuthorizationClient::GetAuthorizationCode(const vector<string> &scopes,
    const string &mailbox, const string &redirectUri)
{
    // build the comma-delimited scope string for sending to the google API
    string scope;
    for (auto scopesIt = scopes.begin(); scopesIt != scopes.end(); scopesIt++)
    {
        if(scopesIt != scopes.begin())
        {
            scope += ",";
        }
        scope += *scopesIt;
    }

    const string oauthUrl = GOOGLE_OAUTH_HOST + AUTH_CODE_ENDPOINT
        + "?response_type=code"
        + "&scope=" + scope
        + "&client_id=" + clientId
        + "&redirect_uri=" + redirectUri
        + "&login_hint=" + mailbox;

    cout << "Please complete the following steps to authorize this application for accessing your gmail account.\n"
        << "1. Copy-and-paste the following URL into your browser and follow the steps on-screen to authorize this application:\n"
        << oauthUrl
        << "\n\n"
        << "2. After Google redirects to " << redirectUri << ", copy-and-paste the whole URL into this console and press Enter.\n"
        << "The URL should look like " << redirectUri << "?code=<46 random characters>\n" << endl;

    cout << "Redirected URL: ";
    string code;
    getline(cin, code);

    size_t pos = code.find("code=");
    if (pos == string::npos)
    {
        throw runtime_error("Could not find code in supplied URL. Are you sure you pasted the correct URL?");
    }

    // 5 is the length of "code=""
    // everything after code= is the code returned by Google
    code = code.substr(pos + 5);

    return code;
}

AccessTokenDetails GoogleAuthorizationClient::ExchangeForAccessToken(const string &authorizationCode, const string &redirectUri)
{
    http_client httpClient(GOOGLE_OAUTH_HOST);

    //the parameters to exchange for access token and refresh token with Google
    string paramsForAccessToken = "";
    paramsForAccessToken += "code=" + authorizationCode + "&";
    paramsForAccessToken += "grant_type=authorization_code&";
    paramsForAccessToken += "client_id=" + clientId + "&";
    paramsForAccessToken += "client_secret=" + clientSecret + "&";
    paramsForAccessToken += "redirect_uri=" + redirectUri;

    http_request accessTokenRequest;
    accessTokenRequest.set_method(methods::POST);
    accessTokenRequest.set_request_uri(ACCESS_TOKEN_ENDPOINT);
    accessTokenRequest.headers().add("Content-Type", "application/x-www-form-urlencoded");
    accessTokenRequest.set_body(paramsForAccessToken);

    http_response httpResponse = httpClient.request(accessTokenRequest).get();

    if(httpResponse.status_code() == status_codes::OK)
    {
        json::value jsonResponse = httpResponse.extract_json().get();

        string accessToken = to_utf8string(jsonResponse["access_token"].as_string());
        string refreshToken = to_utf8string(jsonResponse["refresh_token"].as_string());
        int expiresIn = jsonResponse["expires_in"].as_integer();
        string tokenType = to_utf8string(jsonResponse["token_type"].as_string());

        AccessTokenDetails accessTokenDetails;
        accessTokenDetails.accessToken = accessToken;
        accessTokenDetails.refreshToken = refreshToken;
        accessTokenDetails.tokenType = tokenType;
        accessTokenDetails.expiresIn = expiresIn;

        return accessTokenDetails;
    }
    else
    {
        json::value jsonResponse = httpResponse.extract_json().get();

        ostringstream errorMsgStream;
        errorMsgStream << "Exchange for access token unsuccessful. ";
        errorMsgStream << "HTTP error code: " << httpResponse.status_code() << ". ";
        errorMsgStream << jsonResponse["error"] << ". " << jsonResponse["error_description"] << ".";

        throw runtime_error(errorMsgStream.str());
    }
}
