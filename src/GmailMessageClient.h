#ifndef GMAIL_MESSAGE_CLIENT_H
#define GMAIL_MESSAGE_CLIENT_H

#include <string>
#include <vector>
#include <utility>

/// <summary> A helper struct store IDs that an email message can have. </summary>
struct EmailIds
{
    std::string messageId;
    std::string threadId;
};

/// <summary> A simple (and incomplete) wrapper around Gmail's Messages API. </summary>
class GmailMessageClient
{
public:
    /// <summary> Get a list of messages that fulfil the search query in the specified page. </summary>
    /// <param name="accessToken"> Access token. </param>
    /// <param name="tokenType"> Token type. </param>
    /// <param name="gmailSearchBoxQuery"> The query string. Can be any string that the gmail search box accepts. </param>
    /// <param name="pageToken"> The page ID to retrieve from. </param>
    /// <return>
    /// A pair of vector and string. The vector corresponds to the emails retrieved. The string is the nextPageToken
    /// returned by Google when the number of results is more than what one page can fit. If nextPageToken is empty,
    /// that means the current set of results returned is the last page.
    /// </return>
    std::pair<std::vector<EmailIds>, std::string> ListMessages(const std::string &accessToken, const std::string &tokenType,
        const std::string &gmailSearchBoxQuery, const std::string &pageToken = "");

    /// <summary> Get a single message's details. </summary>
    std::string GetMessageDetails(const std::string &accessToken, const std::string &tokenType,
        const EmailIds &email);

};

#endif
