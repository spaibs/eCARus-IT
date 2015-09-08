# eCARus-Ladefahrrad

Hallo eCARusler,

das ist das offizielle GUI-Ladefahrrad Projekt von eCARus.

## Wie funktioniert GitHub?

Eine kleine Einführung bekommt ihr auf YouTube unter https://www.youtube.com/watch?v=0fKg7e37bQE.

Wenn ihr am Ende des Tages euer aktuelles Projekt speichern wollt, dann müsst ihr wie folgt vorgehen:

- navigiert im Terminal zum eCARus-Ladefahrrad Ordner
- gebt **git add -A** ein
- gebt **git commit -m “MESSAGE“** ein. Ersetzt MESSAGE mit einem Text, was ihr heute verändert habt
- gebt **git push** ein

## Woher bekomme ich mehr Informationen?

Eine ausführliche Dokumentation findest du in unserem eCARus-Wiki unter https://ecarus.wiki.tum.de/Ladefahrrad.

## Wie bekomme ich den Source Code?

Wenn ihr git installiert habt, reicht im Terminal einfach **git clone https://github.com/spaibs/eCARus-Ladefahrrad.git** einzugeben. Dadurch werden die ganzen Dateien in den Ordner eCARus-Ladefahrrad, der sich im aktuellen Verzeichnis befindet, geladen.

## Was für Daten werden geschickt?

Eine Nachricht, egal ob sie zur EB-GUI oder zum PC geschickt wird, ist folgendermaßen aufgebaut:

- zuerst entwerder **g** oder **t**, also entwerder sollen die Daten zur EB-GUI (g) oder zum PC/Tower (t) geschickt werden (1 Byte lang)
- dann eine ID (2 Bytes)
- zum Schluss die Daten (8 Bytes)

Folgende IDs sind implementiert:

- 01: Spannung z.B. g0100000333 -> setzt Spannung auf 333 mV
- 02: Strom z.B. g0200000333 -> setzt Strom auf 333 mA
- 03: Automode ein/aus z.B. g0300000001 für an, g0300000000 für aus
- 04: Lampen ein/aus: z.B. g0400001010 -> schalte Lampe 1 an, Lampe 2 aus, Lampe 3 an, Lampe 4 aus

## Wo finde ich noch mehr von den Icons?

hier: http://people.ubuntu.com/~jr/plasma-next-icons/icons.html
