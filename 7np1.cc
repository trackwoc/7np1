--// MADE BY 7np1 //--

-- Constants
local FOV_COLOR = Color3.fromRGB(255, 255, 0)
local NOTIFICATION_ICON_ID = "rbxassetid://17264955877" -- Asset ID for the notification icon

-- Global Variables
getgenv().Prediction = 0.18
getgenv().Smoothness = 2
getgenv().AimPart = "UpperTorso"
getgenv().OldAimpart = "UpperTorso"
getgenv().ShakeValue = 0
getgenv().AutoPred = false

-- Tool Setup
local Tool = Instance.new("Tool")
Tool.RequiresHandle = false
Tool.Name = "7np1.cc"
Tool.Parent = game.Players.LocalPlayer.Backpack

-- Player Setup
local player = game.Players.LocalPlayer

local function connectCharacterAdded()
    player.CharacterAdded:Connect(onCharacterAdded)
end

connectCharacterAdded()

player.CharacterRemoving:Connect(function()
    Tool.Parent = game.Players.LocalPlayer.Backpack
end)

-- Services
local Players = game:GetService("Players")
local RS = game:GetService("RunService")
local WS = game:GetService("Workspace")
local GS = game:GetService("GuiService")
local SG = game:GetService("StarterGui")

-- Variables
local LP = Players.LocalPlayer
local Mouse = LP:GetMouse()
local Camera = WS.CurrentCamera
local GetGuiInset = GS.GetGuiInset

local AimlockState = true
local Locked
local Victim

local SelectedKey = getgenv().Key
local SelectedDisableKey = getgenv().DisableKey

-- Drawing Setup
local fov = Drawing.new("Circle")
fov.Filled = false
fov.Transparency = 1
fov.Thickness = 1
fov.Color = FOV_COLOR
fov.NumSides = 1000

-- Notification function
function Notify(tx)
    SG:SetCore("SendNotification", {
        Title = "7np1.cc",
        Text = tx,
        Duration = 5,
        Icon = 17264955877 -- Set the icon to the specified asset ID
    })
end

-- Check if aimlock is loaded
if getgenv().Loaded == true then
    Notify("Loaded")
    return
end

getgenv().Loaded = true

-- Utility Functions
function update()
    if getgenv().FOV == true then
        if fov then
            fov.Radius = getgenv().FOVSize * 2
            fov.Visible = getgenv().ShowFOV
            fov.Position = Vector2.new(Mouse.X, Mouse.Y + GetGuiInset(GS).Y)
            return fov
        end
    end
end

function WTVP(arg)
    return Camera:WorldToViewportPoint(arg)
end

function WTSP(arg)
    return Camera.WorldToScreenPoint(Camera, arg)
end

function getClosest()
    local closestPlayer
    local shortestDistance = math.huge

    for i, v in pairs(game.Players:GetPlayers()) do
        local notKO = v.Character:WaitForChild("BodyEffects")["K.O"].Value ~= true
        local notGrabbed = v.Character:FindFirstChild("GRABBING_COINSTRAINT") == nil

        if v ~= game.Players.LocalPlayer and v.Character and v.Character:FindFirstChild("Humanoid") and v.Character.Humanoid.Health ~= 0 and v.Character:FindFirstChild(getgenv().AimPart) and notKO and notGrabbed then
            local pos = Camera:WorldToViewportPoint(v.Character.PrimaryPart.Position)
            local magnitude = (Vector2.new(pos.X, pos.Y) - Vector2.new(Mouse.X, Mouse.Y)).magnitude

            if (getgenv().FOV) then
                if (fov.Radius > magnitude and magnitude < shortestDistance) then
                    closestPlayer = v
                    shortestDistance = magnitude
                end
            else
                if (magnitude < shortestDistance) then
                    closestPlayer = v
                    shortestDistance = magnitude
                end
            end
        end
    end
    return closestPlayer
end

-- Activated Event
Tool.Activated:Connect(function()
    if AimlockState == true then
        Locked = not Locked
        if Locked then
            Victim = getClosest()
            Notify("Locked onto: "..tostring(Victim.Character.Humanoid.DisplayName))
        else
            if Victim ~= nil then
                Victim = nil
                Notify("Unlocked!")
            end
        end
    else
        Notify("Aimlock is not enabled!")
    end
end)

-- Main Loop
RS.RenderStepped:Connect(function()
    update()
    if AimlockState == true then
        if Victim ~= nil then
            local shakeOffset = Vector3.new(
                math.random(-getgenv().ShakeValue, getgenv().ShakeValue),
                math.random(-getgenv().ShakeValue, getgenv().ShakeValue),
                math.random(-getgenv().ShakeValue, getgenv().ShakeValue)
            ) * 0.1
            local LookPosition = CFrame.new(Camera.CFrame.p, Victim.Character[getgenv().AimPart].Position + (Vector3.new(Victim.Character.HumanoidRootPart.Velocity.X,Victim.Character.HumanoidRootPart.AssemblyAngularVelocity.Y*0.5,Victim.Character.HumanoidRootPart.Velocity.Z)*getgenv().Prediction))+shakeOffset
            Camera.CFrame = Camera.CFrame:Lerp(LookPosition, getgenv().Smoothness)
        end
    end
end)

-- Disable default camera connections
for _, con in next, getconnections(workspace.CurrentCamera.Changed) do
    task.wait()
    con:Disable()
end
for _, con in next, getconnections(workspace.CurrentCamera:GetPropertyChangedSignal("CFrame")) do
    task.wait()
    con:Disable()
end

-- Automatic Prediction Adjustments
while task.wait() do
    if getgenv().AutoPred == true then
        pingvalue = game:GetService("Stats").Network.ServerStatsItem["Data Ping"]:GetValueString()
        split = string.split(pingvalue,'(')
        ping = tonumber(split[1])

        if ping < 200 then
            getgenv().Prediction = 0.1973432432343325
        elseif ping < 150 then
            getgenv().Prediction = 0.1922
        elseif ping < 90 then
            getgenv().Prediction = 0.16
        elseif ping < 80 then
            getgenv().Prediction = 0.169
        elseif ping < 70 then
            getgenv().Prediction = 0.1355
        elseif ping < 50 then
            getgenv().Prediction = 0.125
        elseif ping < 40 then
            getgenv().Prediction = 0.12
        elseif ping < 30 then
            getgenv().Prediction = 0.12
        end
    end
end
