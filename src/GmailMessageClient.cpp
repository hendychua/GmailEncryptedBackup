#include "GmailMessageClient.h"
#include "GoogleRestApisConsts.h"

#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/uri.h"

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

pair<vector<EmailIds>, string> GmailMessageClient::ListMessages(const string &accessToken, const string &tokenType,
    const string &gmailSearchBoxQuery, const string &pageToken)
{
    // we need this because date strings contain / and there might be spaces in the query string
    string uriEncodedQueryString = to_utf8string(uri::encode_uri(gmailSearchBoxQuery));

    string fullUriEndpoint = LIST_MESSAGES_ENDPOINT + "?q=" + uriEncodedQueryString;

    if (!pageToken.empty())
    {
        fullUriEndpoint += "&pageToken=" + pageToken;
    }

    http_client apiClient(GOOGLE_APIS_HOST);

    http_request messagesListRequest;
    messagesListRequest.set_method(methods::GET);
    messagesListRequest.set_request_uri(fullUriEndpoint);
    messagesListRequest.headers().add("Authorization", tokenType + " " + accessToken);

    http_response messagesListResponse = apiClient.request(messagesListRequest).get();

    if(messagesListResponse.status_code() == status_codes::OK)
    {
        json::value messagesListJson = messagesListResponse.extract_json().get();

        vector<EmailIds> emailIdsList;
        string nextPageToken = "";

        // the query might return no messages. Hence, we need to check for this.
        if (messagesListJson.has_field("messages"))
        {
            json::array messagesList = messagesListJson["messages"].as_array();
            for (auto messagesListIter = messagesList.begin(); messagesListIter != messagesList.end();
                messagesListIter++)
            {
                EmailIds newEmailIds;
                newEmailIds.messageId = to_utf8string((*messagesListIter)["id"].as_string());
                newEmailIds.threadId = to_utf8string((*messagesListIter)["threadId"].as_string());
                emailIdsList.push_back(newEmailIds);
            }

            // we must check for this field because if it is at the last page, this field will not be present
            if (messagesListJson.has_field("nextPageToken"))
            {
                nextPageToken = to_utf8string(messagesListJson["nextPageToken"].as_string());
            }
        }

        return pair<vector<EmailIds>, string>(emailIdsList, nextPageToken);
    }
    else
    {
        json::value messagesListErrorJson = messagesListResponse.extract_json().get();

        ostringstream errorMsgStream;
        errorMsgStream << "List messages unsuccessful. ";
        errorMsgStream << "HTTP error code: " << messagesListResponse.status_code() << ". ";
        errorMsgStream << messagesListErrorJson["error"] << ". " << messagesListErrorJson["error_description"] << ".";

        throw runtime_error(errorMsgStream.str());
    }
}

string GmailMessageClient::GetMessageDetails(const string &accessToken, const string &tokenType, const EmailIds &email)
{
    string fullUriEndpoint = GET_MESSAGE_DETAILS_ENDPOINT + "/" + email.messageId + "?format=full";

    http_client apiClient(GOOGLE_APIS_HOST);

    http_request getMessageRequest;
    getMessageRequest.set_method(methods::GET);
    getMessageRequest.set_request_uri(fullUriEndpoint);
    getMessageRequest.headers().add("Authorization", tokenType + " " + accessToken);

    http_response getMessageResponse = apiClient.request(getMessageRequest).get();

    if(getMessageResponse.status_code() == status_codes::OK)
    {
        json::value messageJson = getMessageResponse.extract_json().get();

        return to_utf8string(messageJson.serialize());
    }
    else
    {
        json::value getMessageErrorJson = getMessageResponse.extract_json().get();

        ostringstream errorMsgStream;
        errorMsgStream << "Get message details unsuccessful. ";
        errorMsgStream << "HTTP error code: " << getMessageResponse.status_code() << ". ";
        errorMsgStream << getMessageErrorJson["error"] << ". " << getMessageErrorJson["error_description"] << ".";

        throw runtime_error(errorMsgStream.str());
    }
}
