// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;
using System;

public class PokeHunterServerTarget : TargetRules //�� ���� ������Ʈ �̸��� �°� �����մϴ�.
{
    public PokeHunterServerTarget(TargetInfo Target) : base(Target) //�� ���� ������Ʈ �̸��� �°� �����մϴ�.
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("PokeHunter"); //�� ���� ������Ʈ �̸��� �°� �����մϴ�.
    }
}