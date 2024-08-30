#pragma once

/*
C:\Git\aws-sdk-cpp\src\aws-cpp-sdk-core\include;C:\Git\PokeHunter\PokeHunter_Server\packages\AWSSDKCPP-CognitoIdentity.1.6.20140630.25\build\native\include;C:\Git\PokeHunter\PokeHunter_Server\packages\AWSSDKCPP-CognitoIdentityProvider.1.6.20160418.25\build\native\include;C:\Program Files\MySQL\MySQL Server 8.0\include;C:\Git\aws-sdk-cpp\generated\src\aws-cpp-sdk-cognito-idp\include;C:\Git\aws-sdk-cpp\crt\aws-crt-cpp\include;%(AdditionalIncludeDirectories)
*/

#include "stdafx.h"

#include <aws/core/Aws.h>



//#include <aws/core/utils/Outcome.h>
//#include <aws/core/utils/StringUtils.h>
//#include <aws/core/utils/json/JsonSerializer.h>
//#include <aws/core/client/ClientConfiguration.h>
//#include <aws/core/auth/AWSCredentialsProviderChain.h>
//#include <aws/cognito-idp/model/GetUserResult.h>
//#include <aws/cognito-idp/model/GetUserRequest.h>
//#include <aws/cognito-idp/CognitoIdentityProviderClient.h>

//using namespace Aws::Utils;
//using namespace Aws::CognitoIdentityProvider;
//using namespace Aws::CognitoIdentityProvider::Model;

std::string GetPlayerName(std::string& AWSIdToken);
