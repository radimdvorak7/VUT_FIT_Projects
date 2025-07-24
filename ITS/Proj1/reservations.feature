Feature: Manage reservations

Background: 
    Given admin is logged in
    And web browser is at page "Calendar"

# Coverage matrix col.: 12
Scenario: View reservation details
    Given reservation exists
    When admin clicks on reservation
    Then "Reservation details" window is shown

# Coverage matrix col.: 13
Scenario Outline: Delete reservation
    Given "Reservation details" window is shown
    When admin clicls on "Delete"
    Then "Delete Appointment" window is shown
    And <buttons> are shown
        Examples:
        | buttons |
        | Cancel  |
        | Delete  |

# Coverage matrix col.: 14
Scenario: Delete reservation - confirmation
    Given "Delete Appointment" window is shown
    When admin clicls on "Delete"
    Then reservation is deleted

# Coverage matrix col.: 15
Scenario: Delete reservation - cancellation
    Given "Delete Appointment" window is shown
    When admin clicls on "Cancel"
    Then reservation is not deleted
 
# Coverage matrix col.: 16
Scenario: Edit reservation
    Given "Reservation details" window is shown
    When admin clicls on "Edit"
    Then "Edit appoitment" window is shown

# Coverage matrix col.: 17
Scenario Outline: Edit reservation - succesful
    Given "Edit appoitment" window is shown
    When admin edits <details>
    And admin clicks "Save"
    Then reservation is updated
    And admin gets message "Appointment saved successfully."
    Examples:
        | details          |
        | First Name       |
        | Last Name        |
        | Email            |
        | Phone Number     |
        | Start time       |
        | End time         |
        | Status           |

# Coverage matrix col.: 18
Scenario: Edit reservation - cancellation
    Given "Edit appoitment" window is shown
    When admin clicls on "Cancel"
    Then reservation is not updated

# Coverage matrix col.: 19
Scenario Outline: Create reservation
    Given reservation on time slot doesn't exists
    When admin click time slot
    Then "Add New Event" window is shown
    And <buttons> are shown
    Examples:
        | buttons        |
        | Unavailability |
        | Appointment    |

# Coverage matrix col.: 20
Scenario Outline: Create appoitment - new customer
    Given "Add New Event" window is shown
    When admin clicks "Appoitment"
    And admin fills fields <customer-details> with <data>
    And admin click "Save"
    Then reservation is created
    And admin gets message "Appointment saved successfully."
    Examples:
        | customer-details | data               |
        | First Name       | Radim              |
        | Last Name        | Dvorak             |
        | Email            | example@gmail.com  |
        | Phone Number     | 554 455 511        |

# Coverage matrix col.: 21
Scenario: Create appoitment - existing customer
    Given "Add New Event" window is shown
    When admin clicks "Appoitment"
    And admin click "Select"
    And admin click on customer
    And admin click "Save"
    Then reservation is created
    And admin gets message "Appointment saved successfully."

