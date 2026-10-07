// ==========================================
// C++ CROW PROJECT - FRONTEND JAVASCRIPT
// ==========================================

// Crow backend address
const API_BASE = '';

// ---------- helpers ----------
const $ = id => document.getElementById(id);

const views = document.querySelectorAll('.view');

// Current logged-in user
let user = null;
let resetUser = null;


// ==========================================
// VIEW FUNCTIONS
// ==========================================

function showView(id) {

  views.forEach(v => {
    v.classList.toggle('active', v.id === id);
  });

  document.querySelectorAll('.message').forEach(m => {
    m.textContent = '';
    m.className = 'message';
  });

  window.scrollTo(0, 0);
}


function showMsg(id, text, ok = false) {

  const el = $(id);

  el.textContent = text;

  el.className =
    'message ' + (ok ? 'success' : 'error');
}


// ==========================================
// TOAST MESSAGE
// ==========================================

let toastTimer;

function toast(text) {

  const t = $('toast');

  t.textContent = text;

  t.classList.add('show');

  clearTimeout(toastTimer);

  toastTimer = setTimeout(() => {
    t.classList.remove('show');
  }, 2500);
}


// ==========================================
// API FUNCTION
// ==========================================

async function api(path, method = 'GET', data = null) {

  const options = {
    method: method,
    headers: {
      'Content-Type': 'application/json'
    }
  };

  if (data !== null) {
    options.body = JSON.stringify(data);
  }

  const res = await fetch(
    API_BASE + path,
    options
  );

  let body = {};

  try {
    body = await res.json();
  }
  catch (_) {
    body = {};
  }

  if (!res.ok) {

    throw new Error(
      body.message ||
      'Something went wrong. Try again.'
    );

  }

  return body;
}


// ==========================================
// DISABLE BUTTON WHILE REQUEST IS RUNNING
// ==========================================

async function submit(form, fn) {

  const btn = form.querySelector(
    '[type=submit]'
  );

  btn.disabled = true;

  try {
    await fn();
  }
  finally {
    btn.disabled = false;
  }
}


// ==========================================
// NAVIGATION
// ==========================================

const go = id => () => showView(id);


[
  'heroLoginBtn',
  'loginFromSignup',
  'loginFromForgot'
].forEach(i => {

  $(i).onclick = go('loginView');

});


[
  'heroSignupBtn',
  'signupFromLogin'
].forEach(i => {

  $(i).onclick = go('signupView');

});


$('forgotFromLogin').onclick =
  go('forgotView');


$('backFromLogin').onclick =
$('backFromSignup').onclick =
  go('welcomeView');


$('showPasswordForm').onclick = () => {

  $('passwordForm').hidden = false;

  $('showPasswordForm').hidden = true;

};


$('logoLink').onclick = e => {

  e.preventDefault();

  if (!user) {
    showView('welcomeView');
  }

};


// ==========================================
// DASHBOARD NAVIGATION
// ==========================================

function showPage(pageId) {

  document
    .querySelectorAll('.dashboard-page')
    .forEach(p => {

      p.classList.toggle(
        'active-page',
        p.id === pageId
      );

    });


  document
    .querySelectorAll('.nav-btn[data-page]')
    .forEach(b => {

      b.classList.toggle(
        'active',
        b.dataset.page === pageId
      );

    });
}

document
  .querySelectorAll('.nav-btn[data-page]')
  .forEach(b => {

    b.onclick = () => {

      showPage(b.dataset.page);

      // Load saved note when My Notes is opened
      if (b.dataset.page === 'notesPage') {
        loadUserNote();
      }

    };

  });

// ==========================================
// SHOW / HIDE PASSWORD
// ==========================================

document
  .querySelectorAll('.toggle-pw')
  .forEach(btn => {

    btn.onclick = () => {

      const input =
        btn.previousElementSibling;

      const show =
        input.type === 'password';


      input.type =
        show ? 'text' : 'password';


      btn.textContent =
        show ? 'Hide' : 'Show';


      btn.setAttribute(
        'aria-label',
        show
          ? 'Hide password'
          : 'Show password'
      );

    };

  });


// ==========================================
// PASSWORD STRENGTH
// ==========================================

function strength(p) {

  let s = 0;

  if (p.length >= 8)
    s++;

  if (
    /[a-z]/.test(p) &&
    /[A-Z]/.test(p)
  )
    s++;

  if (/\d/.test(p))
    s++;

  if (/[^A-Za-z0-9]/.test(p))
    s++;

  return s;
}


$('signupPassword')
  .addEventListener('input', e => {

    const s =
      strength(e.target.value);

    const bar =
      document.querySelector(
        '.strength i'
      );


    bar.style.width =
      (s * 25) + '%';


    bar.style.background =
      [
        '#c23b3b',
        '#c23b3b',
        '#d98a1f',
        '#1f8a5b',
        '#1f8a5b'
      ][s];


    $('strengthText').textContent =
      e.target.value
        ? [
            'Too weak',
            'Weak',
            'Fair',
            'Good',
            'Strong'
          ][s]
        : 'Use 8+ characters with letters and numbers.';

  });


// ==========================================
// LOGIN HISTORY
// ==========================================

function renderHistory(items) {

  const list =
    $('historyList');

  list.innerHTML = '';


  if (!items || !items.length) {

    list.innerHTML =
      '<li class="muted">No logins yet.</li>';

    return;
  }


  items.forEach(item => {

    const li =
      document.createElement('li');

    // Backend returns:
    // { username, loginTime }

    li.textContent =
      item.loginTime || item;

    list.appendChild(li);

  });

}


// ==========================================
// LOAD LOGIN HISTORY FROM CROW
// ==========================================

async function loadLoginHistory() {

  if (!user)
    return;


  try {

    const result =
      await api(
        '/api/login-history/' +
        encodeURIComponent(
          user.username
        ),
        'GET'
      );


    renderHistory(
      result.history || []
    );

  }
  catch (err) {

    console.error(
      'History error:',
      err
    );

    renderHistory([]);

  }

}


// ==========================================
// SET LOGGED-IN USER
// ==========================================

function setUser(u) {

  user = u;


  const initial =
    (
      u.name ||
      u.username ||
      'U'
    )[0].toUpperCase();


  $('dashboardUsername')
    .textContent =
      u.username;


  $('profileName')
    .textContent =
      u.name;


  $('profileEmail')
    .textContent =
      u.email;


  $('profileUsername')
    .textContent =
      '@' + u.username;


  $('sidebarAvatar')
    .textContent =
      initial;


  $('profileAvatar')
    .textContent =
      initial;


  $('updateName').value =
    u.name;


  $('updateEmail').value =
    u.email;


  showPage(
    'profilePage'
  );


  showView(
    'dashboardView'
  );


  // Get real login history
  // from C++ Stack API
  loadLoginHistory();

}
// ==========================================
// MULTIPLE NOTES SYSTEM
// ==========================================

let editingNoteId = null;


// ==========================================
// LOAD ALL USER NOTES
// ==========================================

async function loadUserNote() {

  if (!user)
    return;

  try {

    const result =
      await api(
        '/api/notes/' +
        encodeURIComponent(user.username),
        'GET'
      );

    renderNotes(
      result.notes || []
    );

  }
  catch (err) {

    showMsg(
      'noteMessage',
      err.message
    );

  }

}


// ==========================================
// DISPLAY NOTES
// ==========================================

function renderNotes(notes) {

  const list =
    $('notesList');

  list.innerHTML = '';


  if (!notes.length) {

    list.innerHTML =
      '<p class="muted">No notes yet. Create your first note.</p>';

    return;
  }


  notes.forEach(note => {

    const card =
      document.createElement('div');

    card.className =
      'note-card';


    const title =
      document.createElement('h3');

    title.textContent =
      note.title;


    const content =
      document.createElement('p');

    content.textContent =
      note.content;


    const actions =
      document.createElement('div');

    actions.className =
      'note-actions';


    // EDIT BUTTON
    const editBtn =
      document.createElement('button');

    editBtn.type =
      'button';

    editBtn.className =
      'secondary-btn';

    editBtn.textContent =
      'Edit';


    editBtn.onclick = () => {

      editingNoteId =
        note.id;

      $('noteTitle').value =
        note.title;

      $('noteContent').value =
        note.content;

      $('noteFormTitle').textContent =
        'Edit Note';

      $('saveNoteBtn').textContent =
        'Update Note';

      $('cancelEditBtn').hidden =
        false;

      $('noteTitle').focus();

    };


    // DELETE BUTTON
    const deleteBtn =
      document.createElement('button');

    deleteBtn.type =
      'button';

    deleteBtn.className =
      'secondary-btn';

    deleteBtn.textContent =
      'Delete';


    deleteBtn.onclick =
      async () => {

        const confirmed =
          confirm(
            'Delete this note?'
          );

        if (!confirmed)
          return;


        try {

          const result =
            await api(
              '/api/notes/' +
              encodeURIComponent(
                user.username
              ) +
              '/' +
              note.id,
              'DELETE'
            );


          showMsg(
            'noteMessage',
            result.message,
            true
          );


          await loadUserNote();

        }
        catch (err) {

          showMsg(
            'noteMessage',
            err.message
          );

        }

      };


    actions.appendChild(
      editBtn
    );

    actions.appendChild(
      deleteBtn
    );


    card.appendChild(
      title
    );

    card.appendChild(
      content
    );

    card.appendChild(
      actions
    );


    list.appendChild(
      card
    );

  });

}


// ==========================================
// CREATE / UPDATE NOTE
// ==========================================

$('saveNoteBtn').onclick =
  async () => {

    if (!user)
      return;


    const title =
      $('noteTitle')
        .value
        .trim();

    const content =
      $('noteContent')
        .value
        .trim();


    if (!title) {

      return showMsg(
        'noteMessage',
        'Please enter a note title.'
      );

    }


    if (!content) {

      return showMsg(
        'noteMessage',
        'Please enter note content.'
      );

    }


    try {

      let result;


      // CREATE NEW NOTE
      if (editingNoteId === null) {

        result =
          await api(
            '/api/notes/' +
            encodeURIComponent(
              user.username
            ),
            'POST',
            {
              title,
              content
            }
          );

      }

      // UPDATE EXISTING NOTE
      else {

        result =
          await api(
            '/api/notes/' +
            encodeURIComponent(
              user.username
            ) +
            '/' +
            editingNoteId,
            'PUT',
            {
              title,
              content
            }
          );

      }


      resetNoteForm();


      showMsg(
        'noteMessage',
        result.message,
        true
      );


      await loadUserNote();

    }
    catch (err) {

      showMsg(
        'noteMessage',
        err.message
      );

    }

  };


// ==========================================
// RESET NOTE FORM
// ==========================================

function resetNoteForm() {

  editingNoteId = null;

  $('noteTitle').value = '';

  $('noteContent').value = '';

  $('noteFormTitle')
    .textContent =
      'Create New Note';

  $('saveNoteBtn')
    .textContent =
      'Create Note';

  $('cancelEditBtn')
    .hidden =
      true;

}


// ==========================================
// CANCEL EDIT
// ==========================================

$('cancelEditBtn').onclick =
  () => {

    resetNoteForm();

    showMsg(
      'noteMessage',
      ''
    );

  };


// ==========================================
// LOGOUT
// ==========================================

$('logoutBtn').onclick = () => {

  user = null;

  showView(
    'welcomeView'
  );

  toast(
    'You have been logged out.'
  );

};


// ==========================================
// LOGIN FORM
// ==========================================

$('loginForm').onsubmit = e => {

  e.preventDefault();


  const username =
    $('loginUsername')
      .value
      .trim();


  const password =
    $('loginPassword')
      .value;


  if (!username || !password) {

    return showMsg(
      'loginMessage',
      'Enter your username and password.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        const u =
          await api(
            '/api/login',
            'POST',
            {
              username,
              password
            }
          );


        setUser(u);


        e.target.reset();

      }
      catch (err) {

        showMsg(
          'loginMessage',
          err.message
        );

      }

    }
  );

};


// ==========================================
// SIGNUP FORM
// ==========================================

$('signupForm').onsubmit = e => {

  e.preventDefault();


  const data = {

    name:
      $('signupName')
        .value
        .trim(),

    email:
      $('signupEmail')
        .value
        .trim(),

    username:
      $('signupUsername')
        .value
        .trim(),

    password:
      $('signupPassword')
        .value

  };


  if (
    !data.name ||
    !data.email ||
    !data.username
  ) {

    return showMsg(
      'signupMessage',
      'Fill in all fields.'
    );

  }


  if (
    data.password.length < 8
  ) {

    return showMsg(
      'signupMessage',
      'Password must be at least 8 characters.'
    );

  }


  if (
    data.password !==
    $('signupConfirmPassword').value
  ) {

    return showMsg(
      'signupMessage',
      'Passwords do not match.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        await api(
          '/api/signup',
          'POST',
          data
        );


        e.target.reset();


        showView(
          'loginView'
        );


        showMsg(
          'loginMessage',
          'Account created. You can log in now.',
          true
        );

      }
      catch (err) {

        showMsg(
          'signupMessage',
          err.message
        );

      }

    }
  );

};


// ==========================================
// FORGOT PASSWORD - VERIFY ACCOUNT
// ==========================================

$('forgotForm').onsubmit = e => {

  e.preventDefault();

  const username =
    $('forgotUsername').value.trim();

  const email =
    $('forgotEmail').value.trim();


  if (!username || !email) {

    return showMsg(
      'forgotMessage',
      'Enter your username and email.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        await api(
          '/api/forgot',
          'POST',
          {
            username,
            email
          }
        );


        // Remember verified account
        // until password reset finishes.
        resetUser = {
          username,
          email
        };


        // Open reset password screen
        showView(
          'resetPasswordView'
        );

      }
      catch (err) {

        showMsg(
          'forgotMessage',
          err.message
        );

      }

    }
  );

};
// ==========================================
// RESET PASSWORD
// ==========================================

$('resetPasswordForm').onsubmit = e => {

  e.preventDefault();


  if (!resetUser) {

    showView('forgotView');

    return;
  }


  const newPassword =
    $('resetNewPassword').value;


  const confirmPassword =
    $('resetConfirmPassword').value;


  if (newPassword.length < 8) {

    return showMsg(
      'resetPasswordMessage',
      'Password must be at least 8 characters.'
    );

  }


  if (newPassword !== confirmPassword) {

    return showMsg(
      'resetPasswordMessage',
      'Passwords do not match.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        await api(

          '/api/reset-password/' +
          encodeURIComponent(
            resetUser.username
          ),

          'PUT',

          {
            email: resetUser.email,
            newPassword
          }

        );


        // Reset form
        e.target.reset();


        // Clear forgot form
        $('forgotForm').reset();


        // Account reset completed
        resetUser = null;


        // Go back to login
        showView(
          'loginView'
        );


        showMsg(
          'loginMessage',
          'Password reset successfully. Please log in with your new password.',
          true
        );

      }
      catch (err) {

        showMsg(
          'resetPasswordMessage',
          err.message
        );

      }

    }
  );

};


// ==========================================
// BACK TO LOGIN FROM RESET PAGE
// ==========================================

$('loginFromReset').onclick = () => {

  resetUser = null;

  $('resetPasswordForm').reset();

  showView(
    'loginView'
  );

};

// ==========================================
// UPDATE PROFILE
// ==========================================

$('updateForm').onsubmit = e => {

  e.preventDefault();


  const name =
    $('updateName')
      .value
      .trim();


  const email =
    $('updateEmail')
      .value
      .trim();


  if (!name || !email) {

    return toast(
      'Name and email cannot be empty.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        await api(

          '/api/profile/update/' +
          encodeURIComponent(
            user.username
          ),

          'PUT',

          {
            name,
            email
          }

        );


        setUser({
          ...user,
          name,
          email
        });


        showPage(
          'profilePage'
        );


        toast(
          'Changes saved.'
        );

      }
      catch (err) {

        toast(
          err.message
        );

      }

    }
  );

};


// ==========================================
// CHANGE PASSWORD
// ==========================================

$('passwordForm').onsubmit = e => {

  e.preventDefault();


  const oldPassword =
    $('oldPassword').value;


  const newPassword =
    $('newPassword').value;


  if (
    newPassword.length < 8
  ) {

    return toast(
      'New password must be at least 8 characters.'
    );

  }


  if (
    newPassword !==
    $('newPassword2').value
  ) {

    return toast(
      'New passwords do not match.'
    );

  }


  submit(
    e.target,

    async () => {

      try {

        await api(

          '/api/password/' +
          encodeURIComponent(
            user.username
          ),

          'PUT',

          {
            oldPassword,
            newPassword
          }

        );


        e.target.reset();


        $('passwordForm')
          .hidden = true;


        $('showPasswordForm')
          .hidden = false;


        toast(
          'Password changed successfully.'
        );

      }
      catch (err) {

        toast(
          err.message
        );

      }

    }
  );

};