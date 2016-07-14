#ifndef ENCRYPTION_STORAGE_MANAGER_H
#define ENCRYPTION_STORAGE_MANAGER_H

#include <string>

class EncryptionStorageManager
{
public:
    /// <summary> Stores payload in encrypted format in the file specified by filename and using userEncryptionKey.
    /// NOTE: this method generates 3 files for every payload. The unencrypted file with filename,
    /// the encrypted file with <filename>-enc and a decrypted file with <filename>-dec.
    /// I'm generating these files to check that my encryption works correctly. In an actual product
    /// release, the unencrypted file and decrypted file should not be generated. </summary>
    void Persist(const std::string &userEncryptionKey, const std::string &filename, const std::string &payload);
private:
    /// <summary> The encryption function. </summary>
    std::string Encrypt(const std::string &userEncryptionKey, const std::string &payload);

    /// <summary> The decryption function. In an actual product release, this function should not be here.
    /// I am using it to generate decrypted files to compare with the unencrypted files so that
    /// I know my encryption works properly. </summary>
    std::string Decrypt(const std::string &userEncryptionKey, const std::string &payload);
};

#endif
