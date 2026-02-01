# Le super projet GPP de Tom

## Contrôles

### Gameplay

- Q / D pour aller à gauche / droite
- Espace pour sauter
- Shit / T pour tirer un projectile
- A pour tirer un homing missile

### Editeur

Pour afficher l'éditeur / ImGui en général, il faut enlever le `#define NO_IMGUI` dans `app.cpp` à la ligne 23.

La super fenêtre d'ImGui apparaît alors, avec une section `Editor`. On peut alors :

- Activer / désactiver l'éditeur
- Choisir si les nouvelles entitiés sont updatées ou pas
- Updater / désactiver l'update des entités
- Choisir le mode de l'éditeur :
  - `Wall` permet de placer des murs
  - `Entity` permet de placer des entités (en choisissant quelle entité placer)
  - `Remove` permet de supprimer ce qu'il se trouve sous le curseur
- Sauvegarder / charger un niveau à partir d'un nom de fichier

## Qu'est-ce qui fonctionne

- L'éditeur in-game avec fonction de sauvegarde / chargement de niveaux
- Les ennemis avec IA moustache (ils appliquent du knockback au joueur à la collision !)
- Le homing missile (pas programmé avec `atan2` dsl j'avais oublié l'existence de cette fonction j'ai utilisé mes tweens à la place)
- Le screenshake
- La caméra qui a un joli smoothing quand elle suit le joueur
- Le muzzle fire (qui est juste une explosion scaled down)
- Les explosions
- Le recul quand le joueur tire

## Qu'est-ce qui ne fonctionne pas

- Les collisions sont un peu cassées (faut faire attention à pas aller trop vite haha)
- Pas de death ray
- Pas de support manette
- Le pet drone existe mais ne sert à rien et n'a pas de collisions avec l'environnement
- Le code est juste moche en général
- CA RAME DE FOU (mais je sais pourquoi c parce que pour chaque collision check on regarde l'entièreté des murs plutôt que juste les murs adjacent à cause de la manière dont sont stockés les murs mais du coup j'ai pas osé changer ça vu qu'il fallait pas changer le code de base)