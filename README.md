

<img src="assets/img/kernix.png" width="300"  alt="knix-logo" align="center"/>   


#  KërNix — *La Maison d’Unix*

> “Recréer les outils Unix emblématiques, à la main, avec une touche locale made in Sénégal.”

KërNix (Kër = “maison” en wolof) est un projet né de la volonté de **comprendre, réinventer et transmettre**.  
Chaque répertoire de ce dépôt est la réécriture d’une commande Unix classique (`cat`, `ls`, `clear`, etc...)  
avec une **valeur ajoutée fonctionnelle**, tout en **respectant la philosophie Unix** :  
> *faire une chose, la faire bien, et la comprendre profondément.*

---

## ⚙️ Philosophie & Démarche

Avant chaque ligne de code, nous procédons à une **étude technique** et un peu de **retro-ingénierie douce**  
pour comprendre le comportement original d’un outil Unix.  
Ensuite, nous le recréons à la main — proprement, lisiblement, et en ajoutant une **plus-value utile**  
sans casser son minimalisme d’origine.

Notre objectif :
- 🌍 **Démocratiser** la compréhension des outils système
- 💡 **Partager** des implémentations simples, libres et réutilisables
- ⚙️ **Encourager** les devs locaux à “toucher au cœur du système”

---

## 🧩 Structure du dépôt

Chaque répertoire correspond à une commande recréée :

KërNix/
├── cat/
├── ls/
├── clear/
├── echo/
└── ... 


> 🧠 Chaque outil dispose de sa propre **page man** (`./docs/manpage/<cmd>.1`)
> consultable directement depuis le terminal.

---

## 🧰 Compilation & Installation

KërNix repose sur un build system **léger et moderne** : [Meson](https://mesonbuild.com/).
Vous pouvez bien sûr utiliser un **Makefile** maison si vous préférez. 😉

### ⚡ Compilation rapide (sans installation)

```bash
$ cd <repertoire_cible>
$ meson setup build
$ meson compile -C build
``` 

###  💾 Installation complète

```bash
$ cd <repertoire_cible>
$ meson setup build
$ meson install -C build
```

> Pour désinstaller  : `ninja  -C build uninstall` 

### Pour consulter la documentation

#### En Mode local (sans installation)

> man  docs/manpage/<nom_cmd.1>

#### Mode après installation

> man <nom_cmd> 


### Auteur & Mainteneur
Umar Ba
jUmarB@protonmail.com
_KerHack-Libre_ — “comprendre, construire, transmettre.”

### ⚖️ Licence

L’ensemble des projets de KërNix sont distribués sous GPLv3,
en accord avec les 4 libertés fondamentales du logiciel libre.

> [!NOTE] 
> Le code est un artisanat. Chez KërNix, on le travaille à la main, avec soin, curiosité et respect du libre.


Discussions, idées et contributions bienvenues sur :
github.com/KerHack-Libre
