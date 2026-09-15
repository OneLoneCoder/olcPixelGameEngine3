# olcPGE 3 for MacOS XCode Project File

<p>
    This is the <a href="https://developer.apple.com/xcode/">XCode</a> project for MacOS.<br/>
    To launch this project in XCode:
</p>

    
<ul>
    <li>
        1:Naviage to <a href="https://github.com/OneLoneCoder/olcPixelGameEngine3">https://github.com/OneLoneCoder/olcPixelGameEngine3</a><br/>
        2: Select the <b>Main</b>  branch <br/>
        3: Click the green <b>Code</b> button dropdown <br/>
        4: Click the <b>Copy</b>  button to copy the git url <br/> 
        <img width="899" height="545" alt="image" src="https://github.com/user-attachments/assets/eda18c2a-c04b-4d33-9a6e-e9ccdab8847d" /><br/>
    </li>
    <li>
        Launch XCode and select <b>Clone Git Repository...</b><br/> 
        <img width="688" height="420" alt="image" src="https://github.com/user-attachments/assets/a1316453-484b-4ee3-9db4-ea4271434f30" /><br/>
    </li>
     <li>
         1 :Enter the repo git link and click <b>Clone</b> <br/> 
        <img width="688" height="420" alt="image" src="https://github.com/user-attachments/assets/2cbd1ba6-6df2-474c-bdde-fbbdca2ec4ff" /> <br/>
        2: At the branch prompt select <b>main</b> <br/>
        3: At the folder prompt select defaults <br/>
    </li>
    <li>
        Open the xcode-macos folder and double click olcPGE3.xcodeproj<br>
        <img width="1536" height="284" alt="image" src="https://github.com/user-attachments/assets/f116d949-c78f-41c9-ae14-50fd898239fd" /> <br/>
        Wait for the project to load<br/>
    </li>
    <li>
        1: Select Project Filter <br/>
        2: Select Project, 
        <br/> - <b>olcPGE</b> is a Multi-header file, builds all headers in the **src** directory
        <br/> - <b>olcPGE_SH</b> is a Single-header file (released version), builds the selected example in the <b>examples</b> directory using olcPixelGameEngine3.h
        <img width="1385" height="888" alt="image" src="https://github.com/user-attachments/assets/12a0e283-5e9b-4a6d-bf33-1bb72f87899f" /> <br/>
        3: Press play and watch the magic happen <br>
    </li>
    <li>
        <hr/>
        **IMPORTANT**<br/> This Xcode project is setup using all the defaults of xcode, therefore the debug/release folders are held within the xcode **Derived Data** folder, hence why the project is setup to auto copy the **assets** folder to the debug/release output. <br/>
        If you use a different setup, please edit the xcode project settings to suit
        <hr/>
    </li>

</ul>

<p>
<ul >
    <b>Some simple rules:</b>
    <li>The olcPGE3.xcodeproj folder and files are all auto generated</li>
    <li>The olcPGE3.xcodeproj folder and files should never be editied outside of XCode itself</li>
    <li>For this solution we will be using XCode Version 14.3.1 (14E300c), if you are making edits to the XCode you must use this version </li>
    <li>If you are using a higher version of XCode Version 14.3.1 (14E300c), and wish to make edit, please raise a PR and attached your olcPGE3.xcodeproj solultion to the PR, the team will then see how best to get your edits in</li>
    <li>Finally olcPGE3.xcodeproj folder is the very same concept as the Solution and Filer files in Visual Studio. Therefore it is important for the team to ensure these files are always stable and can be loaded by a wide range of users. Hence the rules above </li>
</ul>
    Any questions please reach out to us on Discord: <a href="https://discord.com/invite/WhwHUMV">One Lone Coder</a>
</p>

<p>
CopyRight:
    <a href="https://onelonecoder.com"><b>One Lone Coder </b></a>
</p>
