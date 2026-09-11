# Guide d'Installation Détaillé - RP2040 PAL Stepper Controller

## Table des matières
1. [Préparation de l'environnement](#préparation-de-lenvironnement)
2. [Installation du support RP2040](#installation-du-support-rp2040)
3. [Installation des bibliothèques](#installation-des-bibliothèques)
4. [Configuration du hardware](#configuration-du-hardware)
5. [Compilation et programmation](#compilation-et-programmation)
6. [Tests et validation](#tests-et-validation)
7. [Dépannage](#dépannage)

---

## Préparation de l'environnement

### Systèmes d'exploitation supportés
- ✅ Windows 10/11
- ✅ macOS 10.14+
- ✅ Linux (Ubuntu 18.04+, Debian 10+)

### Prérequis
- Arduino IDE 2.0.0 ou supérieur
- USB A vers USB Micro-B (ou USB-C selon votre RP2040-Zero)
- Connexion Internet stable

### Télécharger Arduino IDE

1. Accéder à https://www.arduino.cc/en/software
2. Télécharger la version correspondant à votre système
3. Installer selon les instructions de votre OS
4. Lancer Arduino IDE

---

## Installation du support RP2040

### Étape 1: Accéder aux préférences

**Windows/Linux:**
- Cliquer sur `Fichier` → `Préférences`

**macOS:**
- Cliquer sur `Arduino` → `Préférences`

### Étape 2: Ajouter l'URL du gestionnaire de cartes

1. Localiser le champ: **"URLs de gestionnaire de cartes supplémentaires"**
2. Cliquer sur l'icône rectangle à droite du champ
3. Ajouter l'URL suivante dans une nouvelle ligne:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Cliquer **OK**

### Étape 3: Installer le support RP2040

1. Cliquer sur `Outils` → `Carte` → `Gestionnaire de cartes`
2. Attendre le chargement de la liste
3. Chercher **"Raspberry Pi Pico"** (auteur: Earle F. Philhower)
4. Cliquer sur l'entrée
5. Cliquer le bouton **"Installer"**
6. ⏳ L'installation peut prendre 2-3 minutes (téléchargement des outils)
7. Fermer le gestionnaire quand "INSTALLED" apparaît

### Étape 4: Vérifier l'installation

1. Cliquer sur `Outils` → `Carte`
2. Chercher **"Boards"** section
3. Vous devriez voir **"Raspberry Pi Pico/RP2040"**
4. ✅ C'est correct!

---

## Installation des bibliothèques

### Bibliothèque 1: Adafruit SSD1306 (Écran OLED)

#### Installation via le gestionnaire

1. Cliquer `Sketch` → `Inclure une bibliothèque` → `Gérer les bibliothèques`
2. Chercher **"Adafruit SSD1306"**
3. Sélectionner l'entrée par **Adafruit**
4. Cliquer **"Installer"**
5. ⏳ L'installation peut prendre 1 minute
6. Attendre "Successfully installed"

#### Vérification

- La bibliothèque **Adafruit GFX Library** devrait s'installer automatiquement
- Si ce n'est pas le cas, répéter le processus pour "Adafruit GFX Library"

### Bibliothèque 2: Adafruit GFX Library (dépendance)

Si non installée automatiquement:

1. Même processus que ci-dessus
2. Chercher **"Adafruit GFX Library"**
3. Installer la version la plus récente

### Vérification des versions

Pour vérifier les versions installées:

1. `Sketch` → `Inclure une bibliothèque` → `Gérer les bibliothèques`
2. Chercher **"Adafruit"**
3. Vérifier les versions:
   - Adafruit SSD1306: ≥ 2.5.0
   - Adafruit GFX Library: ≥ 1.10.0

---

## Configuration du hardware

### Sélection de la carte

1. Cliquer `Outils` → `Carte` → `Boards`
2. Sélectionner **"Waveshare RP2040-Zero"**
   (ou **"Raspberry Pi Pico"** si non disponible)

### Configuration des options

Une fois la carte sélectionnée, aller à `Outils` et configurer:

| Option | Valeur | Notes |
|--------|--------|-------|
| **Board** | Waveshare RP2040-Zero | Cible |
| **Port** | COM3 (Windows) ou /dev/ttyACM0 (Linux) | À adapter |
| **USB Stack** | Pico SDK | Standard |
| **Flash Size** | 2MB (264KB SPRAM) | Recommandé |
| **CPU Speed** | 125 MHz | Standard (peut être 200 MHz) |
| **Optimize** | Small (-Os) | Pour économiser la mémoire |
| **RTTI** | Disabled | Pour économiser la mémoire |
| **Stack Protector** | Disabled | Optionnel |

### Détection du port série

1. Connecter le RP2040-Zero via USB
2. Aller à `Outils` → `Port`
3. Une nouvelle entrée devrait apparaître:
   - **Windows**: COM3, COM4, etc.
   - **Linux**: /dev/ttyACM0, /dev/ttyACM1, etc.
   - **macOS**: /dev/cu.usbmodem14201, etc.
4. Sélectionner ce port

**Note:** Si le port n'apparaît pas, voir la section [Dépannage](#dépannage)

---

## Compilation et programmation

### Étape 1: Ouvrir le code

1. Télécharger le fichier `rp2040_pal_stepper_controller.ino`
2. Double-cliquer pour l'ouvrir dans Arduino IDE
3. Ou: `Fichier` → `Ouvrir...` et sélectionner le fichier

### Étape 2: Vérifier la compilation

1. Cliquer sur le bouton **Vérifier** (✓ / Ctrl+R)
   ```
   ✓ Sketch → Vérifier
   ```
2. La console en bas affichera la progression
3. Attendre "Compilation complete"
4. ✅ Pas d'erreurs? On continue!

### Étape 3: Programmer le RP2040-Zero

#### Méthode 1: Via USB directement (recommandée)

1. Connecter le RP2040-Zero au PC avec un câble USB
2. Vérifier que le port est sélectionné (`Outils` → `Port`)
3. Cliquer **Téléverser** (→ / Ctrl+U)
   ```
   Sketch → Téléverser
   ```
4. Attendre les messages:
   ```
   Compiling sketch...
   Uploading...
   Done uploading.
   ```
5. ✅ Fait!

#### Méthode 2: Mode bootloader (si la méthode 1 échoue)

1. Appuyer et **maintenir** le bouton **BOOT** du RP2040-Zero
2. Connecter le RP2040-Zero au PC via USB (bouton toujours appuyé)
3. Relâcher le bouton
4. Un lecteur disque **"RPI-RP2"** doit apparaître
5. Cliquer **Téléverser** dans Arduino IDE
6. Le fichier `.uf2` se copiera automatiquement sur la clé USB

### Étape 4: Vérification du démarrage

1. Ouvrir `Outils` → `Serial Monitor` (Ctrl+Maj+M)
2. Configurer la vitesse à **115200 baud** (coin inférieur droit)
3. Vous devriez voir:
   ```
   === RP2040 PAL Stepper Controller ===
   ...
   Setup completed
   ```
4. ✅ Le RP2040 est prêt!

---

## Compilation et génération du fichier UF2

### Générer le fichier UF2 compilé

Pour obtenir un fichier `.uf2` prêt à être flashé:

1. Cliquer `Sketch` → `Exporter les binaires compilés`
2. Un fichier `.uf2` est créé dans le dossier du sketch
3. Ce fichier peut être copié sur le RP2040-Zero en mode bootloader

### Emplacement du fichier UF2

Le fichier se trouve généralement à:
```
{Dossier du sketch}/build/rp2040_pal_stepper_controller.uf2
```

### Flasher via le fichier UF2

1. Appuyer et maintenir **BOOT**
2. Connecter le RP2040 via USB
3. Relâcher **BOOT**
4. Un lecteur **"RPI-RP2"** apparaît
5. Copier le fichier `.uf2` sur ce lecteur
6. Le RP2040 redémarre automatiquement et exécute le code

---

## Tests et validation

### Test 1: Communication série

1. Ouvrir le **Serial Monitor** (115200 baud)
2. Vérifier que des messages s'affichent
3. Appuyer sur les boutons
4. Observer les messages correspondants

### Test 2: Écran OLED

1. Allumer le système
2. L'écran devrait afficher:
   - **Haut (bleu)**: "0 Hz"
   - **Milieu**: "AUTO" ou "MANUEL"
   - **Bas**: "AVANT"
3. Si rien n'apparaît, voir [Dépannage → Écran OLED](#écran-oled-ne-saffiche-pas)

### Test 3: Encodeur rotatif

1. En mode MANUEL, tourner l'encodeur
2. La fréquence devrait changer (par pas de 100 Hz)
3. Enfoncer le bouton et tourner = pas de 1 Hz
4. Vérifier dans le Serial Monitor

### Test 4: Moteur pas à pas

1. S'assurer que le moteur est branché correctement
2. Appuyer sur le bouton **MARCHE**
3. Le moteur devrait tourner
4. Appuyer sur **ARRÊT** pour arrêter

### Test 5: Signal PAL (optionnel)

1. Connecter une source PAL 50Hz
2. En mode AUTO, appuyer sur **MARCHE**
3. La fréquence devrait se synchroniser à ~10000 Hz
4. Observer la stabilité dans le Serial Monitor

### Checklist de succès

- ✅ Tous les tests réussissent
- ✅ Le Serial Monitor affiche les messages corrects
- ✅ L'écran OLED s'affiche correctement
- ✅ L'encodeur répond aux rotations
- ✅ Le moteur tourne au signal MARCHE
- ✅ Les arrêts fonctionnent correctement

---

## Dépannage

### Port série non détecté

**Symptôme**: Aucun port n'apparaît dans `Outils` → `Port`

**Solutions**:
1. ✓ Vérifier que le câble USB fonctionne (test avec autre appareil)
2. ✓ Essayer un autre port USB
3. ✓ Redémarrer Arduino IDE
4. ✓ Redémarrer l'ordinateur
5. ✓ Installer les drivers CH340 (Windows):
   - Télécharger: https://cdn.shopify.com/s/files/1/0438/4735/2471/files/CH341SER.EXE
   - Installer et redémarrer

### Compilation échoue

**Erreur**: "config.h: No such file or directory"

**Solution**:
- Vérifier que `config.h` est dans le même dossier que le `.ino`

**Erreur**: "Adafruit_SSD1306.h: No such file or directory"

**Solution**:
- Installer la bibliothèque Adafruit SSD1306

### Téléversement échoue

**Solutions**:
1. ✓ Vérifier le port sélectionné
2. ✓ Essayer la Méthode 2 (mode bootloader)
3. ✓ Redémarrer Arduino IDE

### Écran OLED ne s'affiche pas

**Vérifications**:
1. ✓ Alimentation (GND + VCC +3.3V)
2. ✓ Connexions I2C (SDA → GPIO 12, SCL → GPIO 13)
3. ✓ Adresse I2C (0x3C par défaut)

### Moteur ne tourne pas

**Vérifications**:
1. ✓ Alimentation moteur (12V)
2. ✓ Connexions driver (GPIO 8, 9, 10)
3. ✓ Messages dans Serial Monitor

---

**Besoin d'aide?** Consulter le dossier `BROCHAGE.md` ou créer une issue sur GitHub.

