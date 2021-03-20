# microemacs-st
English below.

Port de MicroEmacs pour l'Atari ST et son système d'exploitation TOS, par le groupe MPS.
Il est basé sur MicroEmacs 4.00 et a les spécificités suivantes:
* la cible st52 est mise à jour:
  * utilise la librarie GEM GEMlib du dépôt FreeMiNT car c'est la plus récente et elle est maintenue
  * utilise la librairie C libcmini du dépôt FreeMiNT car elle produit des exécutables plus petits que MiNTlib et on cible le TOS
  * GCC 4.6.4 de Vincent Rivière car c'est un compilateur actuel et assez bon

Comparé au MicroEmacs "officiel", celui ci a les différences fonctionnelles suivantes:
* il est fait pour bien supporter le clavier Atari français, les autres claviers peuvent ou pas bien fonctionner
* l'encryption est désactivée, pour réduire la taille de l'exécutable (voir estruct.h pour réactiver)
* les abbréviations sont désactivées, pour réduire la taille de l'exécutable (voir estruct.h pour réactiver)

----
(english)

Tweaked port of MicroEmacs for the Atari ST's TOS OS, by the MPS Atari enthousiasts. 
It is based on MicroEmacs 4.00and has the following features:
The st52 target is updated to
* use GEMlib (from FreeMiNT repo) as GEM library, as it's the most up to date GEM library and is still maintained
* libcmini (from FreeMiNT repo) as C library, as it allows to make small-sized programs, much smaller than the MiNTlib and is still maintained
* GCC 4.6.4 as it's a decent C compiler

Compared to stock MicroEmacs 4.00 it has the following changes:
* correctly support the FrenchAtari keyboard, mileage with other keyboards may vary 
* The support for encryption is disabled for sake of keeping the executable small (see estruct.h on how to re-enable)
* The support for abbreviations is disabled for sake of keeping the executable small (see estruct.h on how to re-enable)
