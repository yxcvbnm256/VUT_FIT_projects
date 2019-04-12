import * as functions from 'firebase-functions';
import { initializeApp, firestore } from 'firebase-admin';
import { UserRecord } from 'firebase-functions/lib/providers/auth';

initializeApp(functions.config().firebase);

// // Start writing Firebase Functions
// // https://firebase.google.com/docs/functions/typescript
//
// export const helloWorld = functions.https.onRequest((request, response) => {
//  response.send("Hello from Firebase!");
// });

export const createProfile = functions.auth.user().onCreate((user: UserRecord) => {
  const db = firestore();

  console.log(user);

  return db.doc(`users/${user.uid}`).set({
    uid: user.uid,
    email: user.email,
    name: user.displayName || user.email ? user.email.split('@')[0] : '',
    photoURL: user.photoURL,

    address: '',
    phone: user.phoneNumber,
    notificationToken: '',
  });
});
