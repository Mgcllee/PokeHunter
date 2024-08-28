// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class PokeHunterServerTarget : TargetRules //이 행은 프로젝트 이름에 맞게 변경합니다.
{
    public PokeHunterServerTarget(TargetInfo Target) : base(Target) //이 행은 프로젝트 이름에 맞게 변경합니다.
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("PokeHunter"); //이 행은 프로젝트 이름에 맞게 변경합니다.
    }
}