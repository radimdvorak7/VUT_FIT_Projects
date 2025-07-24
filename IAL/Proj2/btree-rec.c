/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree){
  (*tree) = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value){
  if(tree == NULL || (*value) == NULL){
    return false;
  }
  // Klíč je větší
  if(key > tree->key){
    return bst_search(tree->right,key,value);
  }
  // Klíč je menší
  else if(key < tree->key){
    return bst_search(tree->left,key,value);
  }
  // Klíč je stejný
  else if(key == tree->key){
    (*value) = &tree->content;
    return true;
  }
  return false;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, bst_node_content_t value){
  // Strom je prázdný
  if ((*tree) == NULL){
    // Alokace uzlu
    *tree = (bst_node_t *) malloc(sizeof(struct bst_node));
    if (*tree == NULL){
      return;
    }
     // Inicializace uzlu
    (*tree)->content.type = value.type;
    (*tree)->content.value = value.value;
    (*tree)->key = key;
    (*tree)->left = NULL;
    (*tree)->right = NULL;
    return;
  }
  // Klíč je větší
  if (key > (*tree)->key){
    bst_insert(&((*tree)->right), key, value);
  } 
  // Klíč je menší
  else if (key < (*tree)->key){ 
    bst_insert(&((*tree)->left), key, value);
  } 
  // Klíč je stejný
  else if (key == (*tree)->key){
    // Nahradit novou hodnotou
    free((*tree)->content.value);
    (*tree)->content.type = value.type;
    (*tree)->content.value = value.value;
  }
  return;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree){
  if (*tree == NULL) {
		return;
	}
  // Nemá pravého potomka
  if((*tree)->right == NULL){
    // Nahrazení hodnot pro 'target'
    target->key = (*tree)->key;
    target->content.type = (*tree)->content.type;
    target->content.value = (*tree)->content.value;

    bst_node_t* tmp = *tree;
    *tree = (*tree)->left;
    // Uvolnit uzel a hodnotu
    free(tmp->content.value);
    free(tmp);
    return;
  }
  
  bst_replace_by_rightmost(target,&(*tree)->right);
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  if((*tree) == NULL)
    return;
  
  if(key < (*tree)->key){
    bst_delete(&(*tree)->left,key);
  }
  else if(key > (*tree)->key){
    bst_delete(&(*tree)->right,key);
  }
  else if(key == (*tree)->key){
    // Uzel nemá potomky
    if((*tree)->left == NULL && (*tree)->right == NULL){
      // Uvolnit uzel a hodnotu
      free((*tree)->content.value);
      free(*tree);
      *tree = NULL;
      return;
    }
    // Uzel má pravého potomka
    else if((*tree)->left == NULL && (*tree)->right != NULL){
      bst_node_t* tmp = (*tree)->right;
      // Uvolnit uzel a hodnotu
      free((*tree)->content.value);
      free(*tree);
      // Nahrad pravým potomkem
      *tree = tmp;
      return;
    }
    // Uzel má levého potomka
    else if((*tree)->left != NULL && (*tree)->right == NULL){
      bst_node_t* tmp = (*tree)->left;
      // Uvolnit uzel a hodnotu
      free((*tree)->content.value);
      free(*tree);
      // Nahrad levým potomkem
      *tree = tmp;
      return;
    }
    // Uzel má oba potomky
    else if((*tree)->left != NULL && (*tree)->right != NULL){
      // Nahrad nejpravějším uzlem levého podstromu
      bst_replace_by_rightmost(*tree, &(*tree)->left);
      return;
    }
  }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree){
  if(*tree != NULL){

    bst_dispose((&(*tree)->left));
    bst_dispose((&(*tree)->right));

    // Uvolnit uzel a hodnotu
    free((*tree)->content.value);
    free(*tree);
    (*tree) = NULL;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items){
  if(tree != NULL){
    bst_add_node_to_items(tree,items);

    bst_preorder(tree->left,items);

    bst_preorder(tree->right,items);
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items){
  if(tree != NULL){
    bst_inorder(tree->left,items);
  
    bst_add_node_to_items(tree,items);

    bst_inorder(tree->right,items);
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items){
  if(tree != NULL){
    bst_postorder(tree->left,items);

    bst_postorder(tree->right,items);

    bst_add_node_to_items(tree,items);
  }
}
