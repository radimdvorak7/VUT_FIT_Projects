// ***********************************************
// This example commands.js shows you how to
// create various custom commands and overwrite
// existing commands.
//
// For more comprehensive examples of custom
// commands please read more here:
// https://on.cypress.io/custom-commands
// ***********************************************
//
//
// -- This is a parent command --
// Cypress.Commands.add('login', (email, password) => { ... })
//
//
// -- This is a child command --
// Cypress.Commands.add('drag', { prevSubject: 'element'}, (subject, options) => { ... })
//
//
// -- This is a dual command --
// Cypress.Commands.add('dismiss', { prevSubject: 'optional'}, (subject, options) => { ... })
//
//
// -- This will overwrite an existing command --
// Cypress.Commands.overwrite('visit', (originalFn, url, options) => { ... })

Cypress.Commands.add('addCustomer', (firstName, lastName) => {
    cy.get('#first-name').type(firstName);
    cy.get('#last-name').type(lastName);
    cy.get('#email').type('example@gg.com');
    cy.get('#phone-number').type('544456154');

    cy.get('#save-customer').click();
});

Cypress.Commands.add('findCustomer', (name) => {
    cy.get('.customer-row').contains(name);
});

Cypress.Commands.add('deleteCustomer', (name) => {
    cy.findCustomer(name).click();
    cy.get('#delete-customer').click();
    cy.get('.modal-footer > .btn-primary').click();
});

