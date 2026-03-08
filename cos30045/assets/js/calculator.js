/*
 * calculator.js — Appliance Energy Calculator
 *
 * This file handles ALL calculator behaviour: reading inputs, validating them,
 * computing results, and updating the page.  No HTML structure, no CSS rules here.
 * That separation makes each file easier to read and maintain.
 *
 * KEY CONCEPT: This script is loaded AFTER the HTML (the <script> tag is at the
 * bottom of the <body>), so every element it references already exists in the DOM
 * when this code runs.
 */

// ---------------------------------------------------------------------------
// 1. GET REFERENCES TO DOM ELEMENTS
//    document.getElementById('some-id') returns the HTML element with that id.
//    We store them in variables so we don't repeat the lookup every time.
// ---------------------------------------------------------------------------

const form            = document.getElementById('calc-form');
const applianceSelect = document.getElementById('appliance-select');
const wattsInput      = document.getElementById('watts');
const wattsGroup      = document.getElementById('watts-group');
const hoursInput      = document.getElementById('hours');
const tariffInput     = document.getElementById('tariff');

// Result display elements — we will write computed values into these
const resDaily        = document.getElementById('res-daily-kwh');
const resMonthly      = document.getElementById('res-monthly-kwh');
const resYearly       = document.getElementById('res-yearly-kwh');
const resMonthlyCost  = document.getElementById('res-monthly-cost');
const resYearlyCost   = document.getElementById('res-yearly-cost');
const resultsPanel    = document.getElementById('calc-results');

// ---------------------------------------------------------------------------
// 2. APPLIANCE PRESET LOGIC
//    When the user picks a preset from the dropdown, auto-fill the watts field
//    and hide it (since the value is already known).
//    When they choose "Custom", show the watts field so they can type their own.
// ---------------------------------------------------------------------------

applianceSelect.addEventListener('change', function () {
  // 'this.value' is the <option value="..."> of the currently selected item.
  if (this.value === '') {
    // Custom selected — show the manual watts input
    wattsGroup.style.display = 'block';
    wattsInput.value = '';            // clear any old preset value
  } else {
    // A preset was chosen — fill in the watts and hide the manual field
    wattsInput.value = this.value;
    wattsGroup.style.display = 'none';
    clearError(wattsInput, 'watts-error'); // remove any previous error on this field
  }
});

// ---------------------------------------------------------------------------
// 3. VALIDATION HELPER FUNCTIONS
//    Breaking logic into small named functions keeps code readable and avoids
//    repeating the same pattern for every field.
// ---------------------------------------------------------------------------

/**
 * showError — marks a field as invalid and displays its error message.
 * @param {HTMLElement} input      The <input> or <select> element to mark.
 * @param {string}      errorId    The id of the <span class="field-error"> to show.
 */
function showError(input, errorId) {
  input.classList.add('invalid');                                 // red border via CSS
  document.getElementById(errorId).classList.add('visible');     // show the error text
}

/**
 * clearError — removes error styling and hides the error message.
 */
function clearError(input, errorId) {
  input.classList.remove('invalid');
  document.getElementById(errorId).classList.remove('visible');
}

/**
 * validateInputs — checks all fields and returns the three numeric values,
 *                  or null if anything is invalid.
 *
 * Returns: { watts, hours, tariff } on success, or null on failure.
 */
function validateInputs() {
  let valid = true;  // tracks whether ALL fields passed — we check every field
                     // even after the first failure so the user sees all errors at once

  // --- Watts ---
  // parseFloat converts a string like "75" to the number 75.
  // isNaN("hello") === true; isNaN(75) === false.
  const watts = parseFloat(wattsInput.value);
  if (isNaN(watts) || watts <= 0) {
    showError(wattsInput, 'watts-error');
    valid = false;
  } else {
    clearError(wattsInput, 'watts-error');
  }

  // --- Hours ---
  const hours = parseFloat(hoursInput.value);
  if (isNaN(hours) || hours <= 0 || hours > 24) {
    showError(hoursInput, 'hours-error');
    valid = false;
  } else {
    clearError(hoursInput, 'hours-error');
  }

  // --- Tariff (electricity price in cents/kWh) ---
  const tariff = parseFloat(tariffInput.value);
  if (isNaN(tariff) || tariff <= 0) {
    showError(tariffInput, 'tariff-error');
    valid = false;
  } else {
    clearError(tariffInput, 'tariff-error');
  }

  // Return the values as an object if everything is fine, otherwise null
  return valid ? { watts, hours, tariff } : null;
}

// ---------------------------------------------------------------------------
// 4. CALCULATION FUNCTIONS
//    Pure functions — given numbers in, computed numbers out.  No DOM access here.
//    This makes them easy to test mentally (or with unit tests later).
//
//    Formula:
//      Power (W) / 1000 = Power in kW
//      kW × hours/day   = kWh/day   (energy = power × time)
//      kWh/day × 30     ≈ kWh/month
//      kWh/day × 365    = kWh/year
//      kWh × price($/kWh) = cost ($)
//      price in cents → divide by 100 to get dollars
// ---------------------------------------------------------------------------

function calcDailyKWh(watts, hours) {
  return (watts / 1000) * hours;
}

function calcMonthlyCost(dailyKWh, tariffCents) {
  // tariffCents is cents per kWh, so divide by 100 to get dollars per kWh
  return dailyKWh * 30 * (tariffCents / 100);
}

function calcYearlyCost(dailyKWh, tariffCents) {
  return dailyKWh * 365 * (tariffCents / 100);
}

// ---------------------------------------------------------------------------
// 5. DISPLAY HELPER
//    Formats a number to 2 decimal places and writes it into the page.
//    element.textContent sets the plain text of a node — safer than innerHTML
//    when the content is user-derived, as it never interprets HTML tags.
// ---------------------------------------------------------------------------

function displayResult(element, value, unit) {
  element.textContent = value.toFixed(2) + ' ' + unit;
}

// ---------------------------------------------------------------------------
// 6. FORM SUBMIT EVENT LISTENER — ties everything together
//    addEventListener(event, callback) fires the callback whenever the event
//    occurs on the target element.
// ---------------------------------------------------------------------------

form.addEventListener('submit', function (event) {
  // IMPORTANT: by default, submitting a form reloads the page (or navigates
  // to the action URL).  preventDefault() cancels that browser default so our
  // JavaScript can handle the result instead.
  event.preventDefault();

  // Step A: validate — if invalid, stop here; errors are already displayed
  const inputs = validateInputs();
  if (inputs === null) {
    return;  // early return — don't calculate with bad data
  }

  // Step B: compute
  const { watts, hours, tariff } = inputs;  // destructuring: pull named props from object

  const dailyKWh    = calcDailyKWh(watts, hours);
  const monthlyKWh  = dailyKWh * 30;
  const yearlyKWh   = dailyKWh * 365;
  const monthlyCost = calcMonthlyCost(dailyKWh, tariff);
  const yearlyCost  = calcYearlyCost(dailyKWh, tariff);

  // Step C: write results into the DOM
  displayResult(resDaily,       dailyKWh,    'kWh');
  displayResult(resMonthly,     monthlyKWh,  'kWh');
  displayResult(resYearly,      yearlyKWh,   'kWh');
  // toFixed(2) and a '$' prefix for currency values
  resMonthlyCost.textContent = '$' + monthlyCost.toFixed(2);
  resYearlyCost.textContent  = '$' + yearlyCost.toFixed(2);

  // Step D: reveal the results panel by adding the CSS class 'visible'
  // The CSS rule .calc-results.visible sets max-height to allow the panel to
  // animate open (CSS transition handles the smooth slide-down effect).
  resultsPanel.classList.add('visible');
});
