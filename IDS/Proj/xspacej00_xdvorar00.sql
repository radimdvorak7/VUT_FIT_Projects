-- IDS - SQL skript pro vytvoření objektů schématu databáze
-- Autoři:  Jan Špaček xspacej00
--          Radim Dvořák xdvorar00
-- Zadání: 30 - Transfúzní stanice

DROP INDEX idx_odber_datum;

DROP TRIGGER kontrola_data_rezervace;
DROP TRIGGER kontrola_mnozstvi_krve_odber;

DROP SEQUENCE seq_stanoviste;

DROP TABLE Rezervace CASCADE CONSTRAINTS;
DROP TABLE Pozadavek CASCADE CONSTRAINTS;
DROP TABLE Odber CASCADE CONSTRAINTS;
DROP TABLE Zasobuje CASCADE CONSTRAINTS;
DROP TABLE Pozaduje CASCADE CONSTRAINTS;

DROP TABLE Darce CASCADE CONSTRAINTS;
DROP TABLE Stanoviste CASCADE CONSTRAINTS;
DROP TABLE Zasoba CASCADE CONSTRAINTS;
DROP TABLE Nemocnice CASCADE CONSTRAINTS;
DROP TABLE Spotrebitel CASCADE CONSTRAINTS;

CREATE TABLE Darce (
    rodne_cislo CHAR(10)PRIMARY KEY,
    jmeno VARCHAR(30),
    prijmeni VARCHAR(30),
    krevni_skupina VARCHAR(2) NOT NULL,
    telefonni_cislo INT,
    
    -- Overeni ze datum v rodnem cisle je existujici datum a je delitelne 11
    CHECK (
        -- Obsahuje pouze cislice
        REGEXP_LIKE(rodne_cislo, '^[0-9]{9,10}$')
        AND
        -- Platny rok
        TO_NUMBER(SUBSTR(rodne_cislo, 1, 2)) BETWEEN 0 AND 99
        AND 
        -- Platny mesic, o 50 vyssi pro zenz
        (TO_NUMBER(SUBSTR(rodne_cislo, 3, 2)) BETWEEN 1 AND 12 OR 
         TO_NUMBER(SUBSTR(rodne_cislo, 3, 2)) BETWEEN 51 AND 62)
        AND
        -- Platny den
        TO_NUMBER(SUBSTR(rodne_cislo, 5, 2)) BETWEEN 1 AND 31
        -- Delitely 11
        AND (LENGTH(rodne_cislo) = 9 OR MOD(CAST(rodne_cislo AS INT),11) = 0)
    )
);

CREATE TABLE Stanoviste (
    cislo_stanoviste INT PRIMARY KEY,
    obsazenost INT
);

CREATE TABLE Odber (
    ID_odberu INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    rodne_cislo CHAR(10) NOT NULL REFERENCES Darce(rodne_cislo),
    cislo_stanoviste INT REFERENCES Stanoviste(cislo_stanoviste), 
    mnozstvi INT NOT NULL,
    datum DATE NOT NULL
);

CREATE TABLE Zasoba(
    ID_zasoby INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    krevni_skupina VARCHAR(2),
    mnozstvi INT
);

CREATE TABLE Zasobuje(
    cislo_stanoviste INT REFERENCES Stanoviste(cislo_stanoviste),
    ID_zasoby INT NOT NULL REFERENCES Zasoba(ID_zasoby) ON DELETE CASCADE,
    PRIMARY KEY (cislo_stanoviste, ID_zasoby)
);

CREATE TABLE Pozaduje(
    ID_zasoby INT REFERENCES Zasoba(ID_zasoby),
    rodne_cislo CHAR(10) NOT NULL REFERENCES Darce(rodne_cislo) ON DELETE CASCADE,
    PRIMARY KEY (ID_zasoby, rodne_cislo)
);

CREATE TABLE Spotrebitel(
    ID_spotrebitele INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    nazev VARCHAR(30)
);

CREATE TABLE Pozadavek (
    ID_spotrebitele INT REFERENCES Spotrebitel(ID_spotrebitele) ON DELETE CASCADE,
    ID_zasoby INT NOT NULL REFERENCES Zasoba(ID_zasoby),
    mnozstvi INT,
    PRIMARY KEY (ID_spotrebitele, ID_zasoby)
);

CREATE TABLE Nemocnice (
    ID_nemocnice INT PRIMARY KEY REFERENCES Spotrebitel(ID_spotrebitele)
);

CREATE TABLE Rezervace (
    ID_nemocnice INT REFERENCES Nemocnice(ID_nemocnice) ON DELETE CASCADE,
    ID_zasoby INT REFERENCES Zasoba(ID_zasoby),
    mnozstvi INT,
    datum DATE,
    PRIMARY KEY (ID_nemocnice, ID_zasoby)
);

CREATE SEQUENCE seq_stanoviste
START WITH 1
INCREMENT BY 1;

INSERT INTO Stanoviste VALUES (seq_stanoviste.nextval,15);
INSERT INTO Stanoviste VALUES (seq_stanoviste.nextval,10);
INSERT INTO Stanoviste VALUES (seq_stanoviste.nextval,7);

INSERT INTO Darce VALUES ('0404269008','Radim','Dvorak','A',737505351);
INSERT INTO Darce VALUES ('7704063345','Jan','Novak','A',755424241);
INSERT INTO Darce VALUES ('9553080009','Jana','Novotna','0',null);
INSERT INTO Darce VALUES ('0305121234','Jan','Novak','B',729874561);
INSERT INTO Darce VALUES ('9707130004','Eva','Svobodova','AB',722334556);
INSERT INTO Darce VALUES ('0809021345','Tomas','Horak','0',null);
INSERT INTO Darce VALUES ('8751250002','Michaela','Krejci','B',744556677);

INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0404269008',1,400,TO_DATE('27.2.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('7704063345',2,550,TO_DATE('1.3.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0404269008',1,500,TO_DATE('16.3.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('9553080009',3,600,TO_DATE('20.3.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0404269008',1,500,TO_DATE('16.4.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0404269008',1,500,TO_DATE('16.5.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0305121234',1,400,TO_DATE('18.4.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0305121234',2,550,TO_DATE('28.4.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('9707130004',1,410,TO_DATE('16.4.2025','dd.mm.yyyy'));
INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('8751250002',3,600,TO_DATE('22.4.2025','dd.mm.yyyy'));

INSERT INTO Zasoba(krevni_skupina, mnozstvi) VALUES ('0',450);
INSERT INTO Zasoba(krevni_skupina, mnozstvi) VALUES ('A',500);
INSERT INTO Zasoba(krevni_skupina, mnozstvi) VALUES ('AB',400);

INSERT INTO Zasobuje VALUES (1,1);
INSERT INTO Zasobuje VALUES (2,1);
INSERT INTO Zasobuje VALUES (2,3);

INSERT INTO Pozaduje VALUES (1,'0404269008');
INSERT INTO Pozaduje VALUES (2,'7704063345');

INSERT INTO Spotrebitel (nazev) VALUES ('Stredisko 1');

INSERT INTO Spotrebitel (nazev) VALUES ('Fakultni nemocnice');
INSERT INTO Nemocnice VALUES (2);

INSERT INTO Spotrebitel (nazev) VALUES ('Stredisko 2');

INSERT INTO Pozadavek VALUES (1,2,300);
INSERT INTO Pozadavek VALUES (2,1,400);
INSERT INTO Pozadavek VALUES (1,3,450);

INSERT INTO Rezervace VALUES(2,2,800,TO_DATE('26.3.2025','dd.mm.yyyy'));
INSERT INTO Rezervace VALUES(2,3,800,TO_DATE('31.3.2025','dd.mm.yyyy'));

SELECT jmeno, prijmeni, COUNT(ID_odberu) AS pocet_odberu --Zobrazí dárce a jejich počet odběrů po 15.5.2025
FROM darce NATURAL JOIN odber NATURAL JOIN stanoviste
WHERE odber.datum > TO_DATE('15.3.2025','dd.mm.yyyy')
GROUP BY jmeno, prijmeni;

SELECT krevni_skupina, mnozstvi --Zobrazí všechny darové krevní skupiny a jejich množství
FROM darce NATURAL JOIN odber;

SELECT cislo_stanoviste, mnozstvi, datum --Zobrazí na kterých stanicích bude odebíráno kolik krve v jaký den
FROM Stanoviste NATURAL JOIN Odber;

SELECT krevni_skupina, SUM(mnozstvi) AS celkove_mnozstvi --Zobrazí celkové množství odebrané krve podle krevní skupiny
FROM Darce NATURAL JOIN Odber
GROUP BY krevni_skupina;

SELECT datum, SUM(mnozstvi) AS celkem_odebrano -- Zobrazí množství odebrané krve podle data odběru
FROM Odber
GROUP BY datum
ORDER BY datum;

SELECT jmeno, prijmeni, krevni_skupina --Zobraz všechny dárce, kteří mají rezervaci krevní skupiny, kterou požadovali
FROM Darce
WHERE rodne_cislo IN (
    SELECT rodne_cislo
    FROM Pozaduje
    WHERE ID_zasoby IN (
        SELECT ID_zasoby
        FROM Rezervace
    )
);

SELECT nazev --Zobrazí všechny spotřebitele, kteří mají alespoň jeden požadavek
FROM Spotrebitel s
WHERE EXISTS (
    SELECT 1
    FROM Pozadavek p
    WHERE p.ID_spotrebitele = s.ID_spotrebitele
);

-- Omezuje množství darované krve na interval 0-600
CREATE OR REPLACE TRIGGER kontrola_mnozstvi_krve_odber
BEFORE INSERT OR UPDATE ON Odber
FOR EACH ROW
BEGIN
    IF :NEW.mnozstvi < 0 OR :NEW.mnozstvi > 600 THEN
        RAISE_APPLICATION_ERROR(-20001, 'Neplatné množství krve.');
    END IF;
END;
/

INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum) VALUES ('0404269008',1,1000,TO_DATE('27.2.2025','dd.mm.yyyy'));

-- Kontroluje, že rezervace nejdou vytvářet zpětně
CREATE OR REPLACE TRIGGER kontrola_data_rezervace
BEFORE INSERT OR UPDATE ON Rezervace
FOR EACH ROW
BEGIN
    IF :NEW.datum < TRUNC(SYSDATE) THEN
        RAISE_APPLICATION_ERROR(-20001, 'Rezervace nemuze byt vytvorena zpetne.');
    END IF;
END;
/

INSERT INTO Rezervace VALUES(2,1,800,TO_DATE('26.3.2025','dd.mm.yyyy'));

-- Vypíše telefonní čísa všech dárců dané krvní skupiny
CREATE OR REPLACE PROCEDURE rozeslat_vyzvy(p_krevni_skupina IN VARCHAR2) AS
    CURSOR cur_vyzvy IS
        SELECT rodne_cislo, jmeno, prijmeni, krevni_skupina, telefonni_cislo
        FROM Darce
        WHERE krevni_skupina = p_krevni_skupina;
    
    v_darce Darce%ROWTYPE; -- Proměnná pro uchování aktuálního dárce
BEGIN
    OPEN cur_vyzvy;
    
    LOOP
        FETCH cur_vyzvy INTO v_darce;
        EXIT WHEN cur_vyzvy%NOTFOUND;
        
        -- Rozesílání výzvy na telefon
        IF v_darce.telefonni_cislo IS NOT NULL THEN
            DBMS_OUTPUT.PUT_LINE('Blesková výzva pro dárce (telefon): ' || v_darce.jmeno || ' ' || v_darce.prijmeni || ' - ' || v_darce.telefonni_cislo);
        END IF;
        
    END LOOP;
    
    CLOSE cur_vyzvy;
    
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Chyba při volání procedury: ' || SQLERRM);
END rozeslat_vyzvy;
/

BEGIN
    rozeslat_vyzvy('A');
END;
/

-- Vytvoří darcování
CREATE OR REPLACE PROCEDURE vytvorit_darovani_krve(
    p_rodne_cislo IN Darce.rodne_cislo%TYPE,
    p_mnozstvi IN Odber.mnozstvi%TYPE,
    p_datum IN Odber.datum%TYPE
) IS
    v_cislo_stanoviste Stanoviste.cislo_stanoviste%TYPE;
    v_nalezeno NUMBER := 0;
    v_hledane_datum Odber.datum%TYPE := p_datum;
BEGIN
    LOOP
        FOR station IN (
            SELECT cislo_stanoviste
            FROM Stanoviste
            WHERE cislo_stanoviste NOT IN (
                SELECT cislo_stanoviste
                FROM Rezervace
                WHERE datum = v_hledane_datum
            )
            ORDER BY cislo_stanoviste
        ) LOOP
            v_cislo_stanoviste := station.cislo_stanoviste;
            v_nalezeno := 1;
            EXIT;
        END LOOP;

        IF v_nalezeno = 1 THEN
            INSERT INTO Odber (rodne_cislo, cislo_stanoviste, mnozstvi, datum)
            VALUES (p_rodne_cislo, v_cislo_stanoviste, p_mnozstvi, v_hledane_datum);

            COMMIT;
            EXIT; 
        ELSE
            v_hledane_datum := v_hledane_datum + 1;
        END IF;
    END LOOP;

EXCEPTION
    WHEN OTHERS THEN
        RAISE_APPLICATION_ERROR(-20002, 'Error during blood donation creation: ' || SQLERRM);
END vytvorit_darovani_krve;
/

SELECT * FROM Odber;

BEGIN
    vytvorit_darovani_krve('0305121234',400,TO_DATE('18.4.2025','dd.mm.yyyy'));
END;
/

SELECT * FROM Odber;


EXPLAIN PLAN FOR
SELECT jmeno, prijmeni, COUNT(ID_odberu) AS pocet_odberu --Zobrazí dárce a jejich počet odběrů po 15.3.2025
FROM darce NATURAL JOIN odber NATURAL JOIN stanoviste
WHERE odber.datum > TO_DATE('15.3.2025','dd.mm.yyyy')
GROUP BY jmeno, prijmeni;

SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

-- Vztvoření indexu na datum odběru
CREATE INDEX idx_odber_datum ON Odber(datum);

EXPLAIN PLAN FOR
SELECT d.jmeno, d.prijmeni, COUNT(o.ID_odberu) AS pocet_odberu --Zobrazí dárce a jejich počet odběrů po 15.3.2025
FROM darce d
JOIN odber o ON d.rodne_cislo = o.rodne_cislo  -- Explicitní spojení podle ID_odberu
JOIN stanoviste s ON o.cislo_stanoviste = s.cislo_stanoviste
WHERE o.datum > TO_DATE('15.3.2025','dd.mm.yyyy')
GROUP BY d.jmeno, d.prijmeni;

SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

-- Nedokážeme vytvořit uživatele kvůli chybějícím oprávněním
-- CREATE USER XSPACEJ00 IDENTIFIED BY heslo;
-- GRANT SELECT ON Darce TO XSPACEJ00

--SELECT krevni_skupina, SUM(mnozstvi) AS celkove_mnozstvi
--FROM XDVORAR00.Darce NATURAL JOIN XDVORAR00.Odber
--GROUP BY krevni_skupina;

-- Urcí zatíženost dárce na základě darované krve za poslední rok
WITH OdberyPosledniRok AS (
    SELECT
        rodne_cislo,
        SUM(mnozstvi) AS celkove_mnozstvi
    FROM Odber
    WHERE datum >= ADD_MONTHS(SYSDATE, -12)
    GROUP BY rodne_cislo
)
SELECT
    d.jmeno,
    d.prijmeni,
    o.celkove_mnozstvi,
    CASE
        WHEN o.celkove_mnozstvi >= 2000 THEN 'Vysoké množství příspěvku'
        WHEN o.celkove_mnozstvi IS NULL THEN 'Žádný odběr'
        ELSE 'Nízké množství příspěvků'
        WHEN o.celkove_mnozstvi >= 1000 THEN 'Střední množství přísspěvků'
    END AS kategorie
FROM Darce d
LEFT JOIN OdberyPosledniRok o ON d.rodne_cislo = o.rodne_cislo
ORDER BY o.celkove_mnozstvi DESC NULLS LAST;

