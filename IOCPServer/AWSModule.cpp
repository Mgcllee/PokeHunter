#pragma once

#include "AWSModule.h"
#include <regex>

std::string GetPlayerName(std::string& AWSIdToken) {
    AWSIdToken = std::regex_replace(AWSIdToken, std::regex("\n"), "");

    // Access Token 확인용 (Id Token은 작동하지 않음!)
    // std::cout << "Token: " << AWSIdToken << std::endl;

    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = "ap-northeast-2"; // 리전은 원하는 대로 변경 가능

        Aws::CognitoIdentityProvider::CognitoIdentityProviderClient cognitoClient(clientConfig);

        Aws::CognitoIdentityProvider::Model::GetUserRequest request;
        request.WithAccessToken(AWSIdToken);

        auto outcome = cognitoClient.GetUser(request);
        if (outcome.IsSuccess())
        {
            Aws::CognitoIdentityProvider::Model::GetUserResult result = outcome.GetResult();
            std::cout << "Username: " << result.GetUsername() << std::endl;
            return result.GetUsername();
        }
        else
        {
            std::cout << "Failed to get user: " << outcome.GetError().GetMessage() << std::endl;
            return "Token Error";
        }
    }
    Aws::ShutdownAPI(options);

    return "Empty";
}