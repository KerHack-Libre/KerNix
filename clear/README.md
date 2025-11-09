# Cls  - Terminal Cleaner Revisited
------- 

**cls** est une commande de nettoyage de terminal inspirée de `clear(1)`,  
mais pensée pour les développeurs exigeants qui veulent **plus de contrôle visuel** et **moins de friction**.

---

## Présentation

`cls` ne se contente pas d’effacer l’écran.  
Elle permet de **nettoyer intelligemment** le terminal avec :
- L’**effacement partiel ou arrière** ;
- L’**insertion de “gaps”** (espaces vides) pour aérer la sortie ;
- La création d’une **zone “sticky”** persistante (utile pour les prompts, moniteurs, etc.).

---

## Utilisation

```bash
cls [OPTION]... [NUMBER]
cls            # Efface tout le terminal
cls 5          # Efface seulement les 5 dernières lignes
cls -g 3       # Ajoute 3 lignes vides entre deux sections
cls -s 10      # Conserve une zone fixe de 10 lignes en haut
cls -g 2 -s 5  # Combine un espace vide + une zone sticky

``` 

## Options 
| Option   | Description                                   |
| -------- | --------------------------------------------- |
| `-h`     | Affiche l’aide                                |
| `-v`     | Affiche la version                            |
| `-g [n]` | Ajoute `n` lignes vides (par défaut 1)        |
| `-s [n]` | Conserve `n` lignes fixes en haut du terminal |


## Auteur: 
Développé par Umar Ba — KerHack-Libre

## ⚖️ Licence
Libre et ouverte — redistribution et modification autorisées sous les termes de la GPLv3.

> "cls est né de la simplicité de clear, mais pousse l’idée plus loin : comprendre, adapter, et réinventer l’essentiel." 🖤
