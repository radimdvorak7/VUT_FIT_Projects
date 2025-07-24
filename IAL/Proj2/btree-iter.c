/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
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
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value){
  // Prázdný strom nebo hodnota
  if (tree == NULL || value == NULL)
    return false;

  // Pocházení stromem
  while(tree != NULL){
    // Klíč je větší
    if(key > tree->key){
      tree = tree->right;
    }
    // Klíč je menší
    else if (key < tree->key){
      tree = tree->left;
    }
    // Klíč j stejný
    else if(key == tree->key){
     (*value) = &tree->content;
      return true;
    }
    
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
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, bst_node_content_t value){
  if ((*tree) == NULL){
    // Alokace uzlu
    *tree = (bst_node_t *) malloc(sizeof(bst_node_t));
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

  bst_node_t *new = *tree;
  bst_node_t *root_new;
  
  while(new != NULL){
    root_new = new; // Rodič vkládaného uzlu
    // Klíč je menší
    if (key < new->key){
      // Vkládáný uzel patří do levého podstromu
      new = new->left;
    } 
    // Klíč je větší
    else if (key > new->key){
      // Vkládáný uzel patří do pravého podstromu
      new = new->right;
    } 

    // Nalezen už existující uzel
    else if(key == new->key){
      // Nahrad hodnotu uzlu
      free(new->content.value);
      new->content.value = value.value;
      return;
    }else{
      return;
    }
  }

  // Alokace uzlu
  new = (bst_node_t *) malloc(sizeof(bst_node_t));
  if (new == NULL)
    return;

  // Inicializace uzlu
  new->content.type = value.type;
  new->content.value = value.value;
  new->key = key;
  new->left = NULL;
  new->right = NULL;

  // Napojení rodiče na vkládaný uzel
  if (key < root_new->key){
    root_new->left = new; // Uzel je vkládán do levého podstromu
  } else if (key > root_new->key){
    root_new->right = new; // Uzel je vkládán do pravého podstromu
  }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree){
  if (*tree == NULL) {
		return;
	}

	bst_node_t *current = *tree;
	bst_node_t *root_current = NULL;

	// Dokud nebyl nalezen nejpravější uzel
	while (1) {
		// Nalezený nejpravější potomek
		if (current->right == NULL) {
			// Nahrazení hodnot pro 'target'
			target->key = current->key;
			target->content = current->content;

			// Má nahrazený uzel rodiče
			if (root_current != NULL) {
        // Nahrazení uzlu jeho levým potomkem
				root_current->right = current->left;
			} else {
				// Odstranění kořene
				*tree = current->left;
			}
			// Uvolnění nejpravějšího potomka
      free(current->content.value);
			free(current);
			break;
		} else {
			// Přejití do pravého podstromu
			root_current = current;
			current = current->right;
		}
	}
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
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key){
	if (*tree == NULL) {
		return;
	}

	bst_node_t *current = *tree;
	bst_node_t *root_current = NULL;

	// Dokud neodstraní uzel s klíčem
	while (1) {
		// Uzel neexistuje
		if (current == NULL) {
			return;
		}
		char currentKey = current->key;
		// Odstranění uzlu
		if (key == currentKey) {

			// Uzel nemá potomky
			if (current->left == NULL && current->right == NULL) {
				/// Odstranění ukazatele z rodiče
				if (root_current->left->key == current->key) {
					root_current->left = NULL;
				} 
        else if (root_current->right->key == current->key) {
					root_current->right = NULL;
				} 
        // Žádný rodič, odstranění kořene
        else {
          free((*tree)->content.value);
          free(*tree);
					*tree = NULL; 
				}
        free(current->content.value);
				free(current);
			}

			// Uzel má jeden podstrom
			if ((current->left != NULL) ^ (current->right != NULL)) { // XOR
				bst_node_t *replacementNode;
        // Nahrazení levým potomkem
        if(current->left != NULL){
          replacementNode = current->left;
        }
        // Nahrazení pravým potomkem
        else {
          replacementNode = current->right;
        }

				// Odstranění ukazatele z rodiče
				if (root_current->left->key == current->key) {
					root_current->left = replacementNode;
				} 
        else if (root_current->right->key == current->key) {
					root_current->right = replacementNode;
				} 
        // Žádný rodič, tedy se jedná o kořen
        else {
					*tree = replacementNode; 
				}

        free(current->content.value);
				free(current);
        current = NULL;
        return;
			}

			// Uzel má oba podstromy
			if (current->left != NULL && current->right != NULL) {
				bst_replace_by_rightmost(current, &(current->left));
			}
			return;
		}

		// Přechod do pravého podstromu
		if (key > currentKey) {
			root_current = current;
			current = current->right;
		}

		// Přechod do levého podstromu
		if (key < currentKey) {
			root_current = current;
			current = current->left;
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
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree){
	if (*tree == NULL) {
		return;
	}

	// Inicializace zásobníku
	stack_bst_t *stack = (stack_bst_t *)malloc(sizeof(stack_bst_t));
	if (stack == NULL) {
		return;
	}
	stack_bst_init(stack);

	// Přídání kořene stromu do zásobníku
	stack_bst_push(stack, *tree);

	// Dokud není zásobník uzlů prázdný
	while (!stack_bst_empty(stack)) {
    // Získej uzel z vrcholu zásobníku
		bst_node_t *current = stack_bst_pop(stack);
		if (current == NULL) {
			continue;
		}
    // Aktuální uzel má pravého potomka
		if (current->right != NULL) {
			stack_bst_push(stack, current->right);
		}
		// Aktuální uzel má levého potomka
		if (current->left != NULL) {
			stack_bst_push(stack, current->left);
		}

		// Uvolnění paměti
    //free(current->content.value);
		free(current);
    current = NULL;
	}

	*tree = NULL;
	free(stack);
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items){
  while(tree != NULL){
    stack_bst_push(to_visit,tree);
    bst_add_node_to_items(tree,items);
    tree = tree->left;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items){
  // Inicializace zásobníku
  stack_bst_t *stack = (stack_bst_t *)malloc(sizeof(stack_bst_t));
  stack_bst_init(stack);

  // Zavolání pomocné funkce na celý strom
  bst_leftmost_preorder(tree,stack,items);

  // Dokud není zásobník prázdný
  while(!stack_bst_empty(stack)){
    tree = stack_bst_pop(stack);
    // Zavolání pomocné funkce na pravý podstrom
    bst_leftmost_preorder(tree->right,stack,items);
  }

  free(stack);
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit){
  while(tree != NULL){
    stack_bst_push(to_visit,tree);
    tree = tree->left;
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items){
  // Inicializace zásobníku
  stack_bst_t *stack = (stack_bst_t *)malloc(sizeof(stack_bst_t));
  stack_bst_init(stack);

  // Zavolání pomocné funkce na celý strom
  bst_leftmost_inorder(tree,stack);

  // Dokud není zásobník prázdný
  while (!stack_bst_empty(stack)){
    // Získej uzel z vrcholu zásobníku
    tree = stack_bst_pop(stack); 
    bst_add_node_to_items(tree, items);
    // Zavolání pomocné funkce na pravý podstrom
    bst_leftmost_inorder(tree->right, stack);
  }
  free(stack);
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit){
  while (tree != NULL){
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left;
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items){
  // Inicializace zásobníku uzlů
  stack_bst_t *bst_stack = (stack_bst_t *)malloc(sizeof(stack_bst_t));
  stack_bst_init(bst_stack);

  // Inicializace zásobníku boolů
  stack_bool_t *bool_stack = (stack_bool_t *)malloc(sizeof(stack_bool_t));
  stack_bool_init(bool_stack);

  // Zavolání pomocné funkce na celý strom
  bst_leftmost_postorder(tree,bst_stack,bool_stack);

  bool from_left;

  // Dokud není zásobník uzlů prázdný
  while (!stack_bst_empty(bst_stack)){
    // Získej uzel z vrcholu zásobníku uzlů
    tree = stack_bst_top(bst_stack);
    // Získej hodnotu z vrcholu zásobníku bool
    from_left = stack_bool_pop(bool_stack);

    // Jestli přišel z levého podstromu
    if (from_left) {
      // Ulož na zásobník hodnotu false
      stack_bool_push(bool_stack, false);
      // Zavolej pomocnou funkci na pravý podstrom
      bst_leftmost_postorder(tree->right, bst_stack, bool_stack);
    } 
    else {
      // Získej uzel z vrcholu zásobníku uzlů a ulož
      stack_bst_pop(bst_stack);
      bst_add_node_to_items(tree, items);
    }
  }

  free(bool_stack);
  free(bst_stack);
}
