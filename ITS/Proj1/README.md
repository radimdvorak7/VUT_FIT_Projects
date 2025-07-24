# ITS Projekt 1

- **Autor:** Radim Dvořák (xdvorar00)
- **Datum:** 2025-04-13

## Matice pokrytí artefaktů

| Stránka                 |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  |
| ----------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Domovská stránka        |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Informace o zákazníkovi |     |  x  |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Potvrzení rezervace     |     |     |     |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Správa zákazníků        |     |     |     |     |  x  |  x  |  x  |  x  |  x  |  x  |  x  |     |     |     |     |     |     |     |     |     |     |
| Kalendář                |     |     |     |     |     |     |     |     |     |     |     |  x  |     |     |     |     |     |     |  x  |     |     |
| Detaily rezervace       |     |     |     |     |     |     |     |     |     |     |     |     |  x  |     |     |  x  |     |     |     |     |     |
| Odstranit rezervaci     |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |     |     |     |     |     |     |
| Upravit rezervaci       |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |     |     |     |
| Přidat rezervaci        |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |

## Matice pokrytí aktivit

|       Aktivita                |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  |
| ----------------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Vybírání data temínu          | x   |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Vyplnění informací zákazníka  |     |  x  |  x  |     |     |     |  x  |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Potvrzení rezervace           |     |     |     |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Správa zákazníků              |     |     |     |     |  x  |  x  |  x  |  x  |  x  |     |     |     |     |     |     |     |     |     |     |     |     |
| Přidání zákazníka             |     |     |     |     |     |     |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Smazání zákazníka             |     |     |     |     |     |     |     |     |  x  |     |     |     |     |     |     |     |     |     |     |     |     |
| Vyhledání zákazníka           |     |     |     |     |     |     |     |     |     |  x  |  x  |     |     |     |     |     |     |     |     |     |     |
| Informace o rezervaci         |     |     |     |     |     |     |     |     |     |     |     |  x  |     |     |     |     |     |     |     |     |     |
| Správa rezervací              |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |  x  |  x  |  x  |  x  |  x  |     |     |
| Smazání rezervace             |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |     |     |     |     |     |     |
| Vytvoření rezervace           |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |
| Kontrola: Změna zákazníka     |     |     |     |     |     |     |     |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Kontrola: Změna rezervace     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |     |     |     |

## Matice Feature-Test

|  Feature soubor        |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  |
| ---------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| appoitment.feature     |  x  |  x  |  x  |  x  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| customers.feature      |     |     |     |     | x   | x   | x   | x   |  x  |  x  |  x  |     |     |     |     |     |     |     |     |     |     |
| reservations.feature   |     |     |     |     |     |     |     |     |     |     |     |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |
