use("fitness_tracker");

function applyValidator(collectionName, validator) {
  if (!db.getCollectionNames().includes(collectionName)) {
    db.createCollection(collectionName, validator);
  } else {
    db.runCommand({
      collMod: collectionName,
      validator: validator.validator,
      validationLevel: validator.validationLevel,
      validationAction: validator.validationAction
    });
  }
}

applyValidator("users", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["_id", "login", "name", "surname", "email", "password_hash", "profile", "preferences", "created_at"],
      additionalProperties: true,
      properties: {
        _id: { bsonType: "string", pattern: "^user_[0-9]{3}$" },
        login: { bsonType: "string", minLength: 3, maxLength: 64 },
        name: { bsonType: "string", minLength: 1, maxLength: 128 },
        surname: { bsonType: "string", minLength: 1, maxLength: 128 },
        email: { bsonType: "string", pattern: "^[^@\\s]+@[^@\\s]+\\.[^@\\s]+$" },
        password_hash: { bsonType: "string", minLength: 8 },
        profile: {
          bsonType: "object",
          required: ["gender", "birth_year", "height_cm", "weight_kg"],
          properties: {
            gender: { enum: ["male", "female", "other"] },
            birth_year: { bsonType: "int", minimum: 1940, maximum: 2020 },
            height_cm: { bsonType: "int", minimum: 100, maximum: 230 },
            weight_kg: { bsonType: ["double", "int"], minimum: 30, maximum: 250 }
          }
        },
        preferences: {
          bsonType: "object",
          required: ["goals", "units"],
          properties: {
            goals: {
              bsonType: "array",
              minItems: 1,
              items: { bsonType: "string" }
            },
            units: { enum: ["metric", "imperial"] }
          }
        },
        created_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

applyValidator("exercises", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["_id", "name", "description", "muscle_group", "equipment", "difficulty", "created_at"],
      properties: {
        _id: { bsonType: "string", pattern: "^exercise_[0-9]{3}$" },
        name: { bsonType: "string", minLength: 1, maxLength: 128 },
        description: { bsonType: "string" },
        muscle_group: { bsonType: "string", minLength: 1, maxLength: 64 },
        equipment: {
          bsonType: "array",
          items: { bsonType: "string" }
        },
        difficulty: { bsonType: "int", minimum: 1, maximum: 5 },
        created_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

applyValidator("workouts", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["_id", "user_id", "date", "duration", "exercises", "tags", "created_at"],
      properties: {
        _id: { bsonType: "string", pattern: "^workout_[0-9]{3,}$" },
        user_id: { bsonType: "string", pattern: "^user_[0-9]{3}$" },
        date: { bsonType: "date" },
        duration: { bsonType: ["int", "long"], minimum: 1, maximum: 600 },
        exercises: {
          bsonType: "array",
          minItems: 1,
          items: {
            bsonType: "object",
            required: ["exercise_id", "name", "sets", "reps", "weight"],
            properties: {
              exercise_id: { bsonType: "string", pattern: "^exercise_[0-9]{3}$" },
              name: { bsonType: "string", minLength: 1 },
              sets: { bsonType: ["int", "long"], minimum: 1, maximum: 20 },
              reps: { bsonType: ["int", "long"], minimum: 1, maximum: 500 },
              weight: { bsonType: ["double", "int"], minimum: 0 }
            }
          }
        },
        tags: {
          bsonType: "array",
          items: { bsonType: "string" }
        },
        notes: { bsonType: "string" },
        created_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

applyValidator("auth_sessions", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["_id", "token", "user_id", "expires_at", "created_at"],
      properties: {
        _id: { bsonType: "string", pattern: "^session_[0-9]{3}$" },
        token: { bsonType: "string", minLength: 8 },
        user_id: { bsonType: "string", pattern: "^user_[0-9]{3}$" },
        expires_at: { bsonType: "date" },
        created_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

db.users.createIndex({ login: 1 }, { unique: true });
db.users.createIndex({ email: 1 }, { unique: true });
db.users.createIndex({ name: 1, surname: 1 });
db.exercises.createIndex({ name: 1, muscle_group: 1 }, { unique: true });
db.exercises.createIndex({ muscle_group: 1 });
db.workouts.createIndex({ user_id: 1, date: -1 });
db.workouts.createIndex({ "exercises.exercise_id": 1 });
db.auth_sessions.createIndex({ token: 1 }, { unique: true });
db.auth_sessions.createIndex({ expires_at: 1 });

function expectValidationError(label, action) {
  try {
    action();
    print(label + ": ERROR, invalid document was inserted");
  } catch (error) {
    print(label + ": OK, validation rejected invalid document");
  }
}

expectValidationError("invalid user email", () => {
  db.users.insertOne({
    _id: "user_999",
    login: "bad",
    name: "Bad",
    surname: "Email",
    email: "not-email",
    password_hash: "hash_bad_user",
    profile: { gender: "male", birth_year: 1991, height_cm: 180, weight_kg: 80 },
    preferences: { goals: ["strength"], units: "metric" },
    created_at: new Date()
  });
});

expectValidationError("invalid workout duration", () => {
  db.workouts.insertOne({
    _id: "workout_999",
    user_id: "user_001",
    date: new Date(),
    duration: -10,
    exercises: [{ exercise_id: "exercise_001", name: "Barbell Squat", sets: 1, reps: 1, weight: 0 }],
    tags: ["test"],
    created_at: new Date()
  });
});
