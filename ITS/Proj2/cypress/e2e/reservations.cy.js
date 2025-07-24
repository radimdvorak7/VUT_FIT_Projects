describe('Manage reservations', () => {

  it('View reservation details', () => {
    // @Given
    cy.visit('http://localhost:8080/');

    cy.get('.backend-link').click();
    
    cy.get('#username').type('admin');
    cy.get('#password').type('admin123');

    cy.get('#login').click();

    cy.url().should('match', /\/index\.php\/calendar$/);


    cy.get('.fc-next-button > .fc-icon').click();
    cy.get(':nth-child(40) > .fc-timegrid-slot-lane').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    cy.get('#select-customer').click();
    cy.get('#existing-customers-list > div').click();
    cy.get('#save-appointment').click();
    
    // @When
    cy.get('.fc-event-title-container > .fc-event-title').click();
    
    // @Then

    // Teardown
    cy.get('.delete-popover').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();
  })

  it('Delete reservation', () => {
    // @Given
    cy.visit('http://localhost:8080/');

    cy.get('.backend-link').click();
    
    cy.get('#username').type('admin');
    cy.get('#password').type('admin123');

    cy.get('#login').click();

    cy.url().should('match', /\/index\.php\/calendar$/);

    cy.get('.fc-next-button > .fc-icon').click();
    cy.get(':nth-child(55) > .fc-timegrid-slot-lane').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    cy.get('#select-customer').click();
    cy.get('#existing-customers-list > div').click();
    cy.get('#save-appointment').click();

    // @When
    cy.get('.fc-event-title-container > .fc-event-title').click();
    cy.get('.delete-popover').click();

    // @Teardown
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();
  })

  it('Delete reservation - confirmation', () => {

    // @Given
    cy.visit('http://localhost:8080/');

    cy.get('.backend-link').click();
    
    cy.get('#username').type('admin');
    cy.get('#password').type('admin123');

    cy.get('#login').click();

    cy.url().should('match', /\/index\.php\/calendar$/);

    cy.get('.fc-next-button > .fc-icon').click();
    cy.get(':nth-child(50) > .fc-timegrid-slot-lane').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    cy.get('#select-customer').click();
    cy.get('#existing-customers-list > div').click();
    cy.get('#save-appointment').click();

    // @When
    cy.get('.fc-event-title-container > .fc-event-title').click();
    cy.get('.delete-popover').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();
  })

  it('Delete reservation - cancellation', () => {
    // @Given
    cy.visit('http://localhost:8080/');

    cy.get('.backend-link').click();
    
    cy.get('#username').type('admin');
    cy.get('#password').type('admin123');

    cy.get('#login').click();

    cy.url().should('match', /\/index\.php\/calendar$/);

    cy.get('.fc-next-button > .fc-icon').click();
    cy.get(':nth-child(45) > .fc-timegrid-slot-lane').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    cy.get('#select-customer').click();
    cy.get('#existing-customers-list > div').click();
    cy.get('#save-appointment').click();

    // @When
    cy.get('.fc-event-title-container > .fc-event-title').click();
    cy.get('.delete-popover').click();
    cy.get('#message-modal > .modal-dialog > .modal-content > .modal-footer > .btn-primary').click();

    // Teardown

    cy.get('.fc-event-title-container > .fc-event-title').click();
    cy.get('.delete-popover').click();
    cy.get('.modal-footer > .btn-outline-primary').click();
  })
})