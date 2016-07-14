#if !defined(CLIENT_ID)
#error CLIENT_ID is not defined during compilation.
#endif

#if !defined(CLIENT_SECRET)
#error CLIENT_SECRET is not defined during compilation.
#endif

#include "SetCinEcho.h"

#include "GoogleAuthorizationClient.h"
#include "GmailMessageClient.h"

#include "EncryptionStorageManager.h"

#include "boost/filesystem.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

void showUsage(const char* name)
{
    cerr << "Incorrect number of arguments.\n"
        << "Usage: " << name << " <mailbox> <start date> <end date>\n"
        << "mailbox: the gmail account to backup from.\n"
        << "start date: start of retrieval period in yyyy/mm/dd format.\n"
        << "end date: end of retrieval period in yyyy/mm/dd format.\n";
}

string getUserEncryptionKey()
{
    string key = "";
    string confirmKey = "";

    // Disable echoing for cin because we are going to ask for secret key for encryption.
    SetCinEcho(false);

    do
    {
        cout << "Enter key (case-sensitive) for encryption: ";
        getline(cin, key);
        cout << endl;

        cout << "Enter key again for confirmation: ";
        getline(cin, confirmKey);
        cout << endl;

        if (key != confirmKey)
        {
            cout << "Keys do not match! Enter again." << endl;
        }

    } while(key != confirmKey);

    // Set echoing back for cin.
    SetCinEcho(true);

    return key;
}

int main(const int argc, const char** argv)
{
    if (argc != 4)
    {
        showUsage(argv[0]);
        return 1;
    }

    try
    {
        const string mailbox(argv[1]);
        const string startDateString(argv[2]);
        const string endDateString(argv[3]);

        // a fake redirectUri since we just need to get a code. Nothing should be running on this port.
        const string redirectUri = "http://localhost:1234/";

        const string clientId = CLIENT_ID;
        const string clientSecret = CLIENT_SECRET;

        string userEncryptionKey = getUserEncryptionKey();

        vector<string> scopes(1);
        scopes[0] = "https://www.googleapis.com/auth/gmail.readonly";

        GoogleAuthorizationClient authClient(clientId, clientSecret);

        // Get the auth code from the user.
        string code = authClient.GetAuthorizationCode(scopes, mailbox, redirectUri);

        // Exchange the auth code for access token, which is required for API calls.
        AccessTokenDetails accessTokenDetails = authClient.ExchangeForAccessToken(code, redirectUri);
        // cout << "accessToken: " << accessTokenDetails.accessToken << endl;
        // cout << "refreshToken: " << accessTokenDetails.refreshToken << endl;
        // cout << "tokenType: " << accessTokenDetails.tokenType << endl;
        // cout << "expiresIn: " << accessTokenDetails.expiresIn << endl;

        GmailMessageClient gmailMessageClient;

        string gmailSearchBoxQuery = "";
        gmailSearchBoxQuery += "after:" + startDateString + " ";
        gmailSearchBoxQuery += "before:" + endDateString;

        // Because the query might return results that span more than one page,
        // we have to check for the nextPageToken. If it is empty, that means we
        // have reached the last page. If it is not empty, we will retrieve the next page.

        string pageToken = "";

        vector<EmailIds> allEmails;
        do
        {
            // Get the list of messages that fulfils the time span queried.
            pair<vector<EmailIds>, string> listMessagesResults =
                gmailMessageClient.ListMessages(accessTokenDetails.accessToken, accessTokenDetails.tokenType,
                    gmailSearchBoxQuery, pageToken);

            // combine the current list with the lists retrieved earlier.
            allEmails.reserve(allEmails.size() + listMessagesResults.first.size());
            allEmails.insert(allEmails.end(), listMessagesResults.first.begin(), listMessagesResults.first.end());

            pageToken = listMessagesResults.second;

        } while(!pageToken.empty());

        cout << "Messages retrieved for query: " << allEmails.size() << endl;

        // we will generate the backups in a folder called backups in the working directory.
        boost::filesystem::path backupsDir("backups");
        boost::filesystem::create_directory(backupsDir);

        EncryptionStorageManager encryptionStorageManager;

        // for every email ID retrieved, we get the full email details
        for (size_t i = 0; i < allEmails.size(); i++)
        {
            string emailDetailsJsonString =
                gmailMessageClient.GetMessageDetails(accessTokenDetails.accessToken, accessTokenDetails.tokenType, allEmails[i]);
            // use the messageId as the filename.
            string filename = (backupsDir / allEmails[i].messageId).string();
            encryptionStorageManager.Persist(userEncryptionKey, filename, emailDetailsJsonString);
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
