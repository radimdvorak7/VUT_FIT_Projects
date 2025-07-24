Feature: Manage customers

Background: 
    Given admin is logged in
    And web browser is at page "Customers"

# Coverage matrix col.: 5
Scenario Outline: Not selected customer
    When customer is not selected from "Customers" list
    Then "Add" button is available
    And "Edit" and "Delete" buttons are not available
    And <details> fields are empty
    Examples:
		| details         |
		| First Name      |
		| Last Name       |
		| Email           | 
		| Phone Number    |
        | Adress          |
        | City            |
        | Zip code        |
        | Language        |
        | Timezone        |

# Coverage matrix col.: 6
Scenario Outline: Select customer
    When customer is selected from "Customers" list
    Then <details> fields are shown filled with <data>
    Examples:
		| details         | data                |
		| First Name      | Gordon              |
		| Last Name       | Freeman             |
		| Email           | gordon@testco.local |
		| Phone Number    | +1 (000) 000-0000   |
        | Adress          |                     |
        | City            |                     |
        | Zip code        |                     |
        | Language        | English             |
        | Timezone        | UTC                 |

# Coverage matrix col.: 7
Scenario Outline: Add customer
	When admin adds new customer with the <required> fields filled with <data>
	Then new customer "Radim Dvorak" is added
	Examples:
		| required        | data              |
		| First Name      | Radim             |
		| Last Name       | Dvorak            |
		| Email           | example@gmail.com |
		| Phone Number    | 554 455 511       |
        | Language        | Czech             |
        | Timezone        | UTC               |

# Coverage matrix col.: 8
Scenario: Edit customer
    Given customer "Gordon Freeman" is selected from "Customers" list
    When admin clicks on "Edit"
    And admin changes information about customer "Gordon Freeman"
	Then customer "Gordon Freeman" is updated

# Coverage matrix col.: 9
Scenario: Delete customer
    Given customer "Gordon Freeman" is selected from "Customers" list
    When admin clicks on "Delete"
	Then customer "Gordon Freeman" is deleted

# Coverage matrix col.: 10
Scenario: Search customer - exists
    Given customer exists with "Gor" in their name
    When admin searches for "Gor"
	Then customers with "Gor" in their name are shown

# Coverage matrix col.: 11
Scenario: Search customer - doesn't exist
    Given customer doesn't exist with "Some" in their name
    When amin searches for "Some"
	Then no customers are shown