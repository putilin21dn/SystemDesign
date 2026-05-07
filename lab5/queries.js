use("fitness_tracker");

db.workouts.deleteMany({ _id: "workout_011" });
db.users.deleteMany({ _id: "user_011" });

print("=== CREATE ===");
db.users.insertOne({
  _id: "user_011",
  login: "test_user",
  name: "Test",
  surname: "User",
  email: "test_user@mail.test",
  password_hash: "hash_test_user",
  profile: { gender: "other", birth_year: 1994, height_cm: 172, weight_kg: 70 },
  preferences: { goals: ["strength"], units: "metric" },
  created_at: new Date()
});

db.workouts.insertOne({
  _id: "workout_011",
  user_id: "user_011",
  date: ISODate("2026-04-20T18:00:00Z"),
  duration: 50,
  exercises: [
    { exercise_id: "exercise_002", name: "Bench Press", sets: 3, reps: 10, weight: 60 }
  ],
  tags: ["push"],
  notes: "Created from queries.js",
  created_at: new Date()
});

printjson(db.users.findOne({ _id: "user_011" }));
printjson(db.workouts.findOne({ _id: "workout_011" }));

print("=== READ: equality and projection ===");
printjson(db.users.findOne(
  { login: "runner_anna" },
  { password_hash: 0 }
));

print("=== READ: $eq, $ne, $gt, $lt, $in ===");
printjson(db.exercises.find({
  muscle_group: { $eq: "legs" },
  difficulty: { $gt: 2, $lt: 5 },
  name: { $ne: "Lunge" }
}).toArray());

printjson(db.workouts.find({
  duration: { $gt: 45 },
  tags: { $in: ["strength", "pull"] }
}).toArray());

print("=== READ: $and and $or ===");
printjson(db.users.find({
  $and: [
    { "profile.birth_year": { $lt: 2000 } },
    {
      $or: [
        { "preferences.goals": "strength" },
        { "preferences.goals": "endurance" }
      ]
    }
  ]
}, { password_hash: 0 }).toArray());

print("=== UPDATE: scalar fields ===");
db.users.updateOne(
  { _id: "user_011" },
  {
    $set: {
      "profile.weight_kg": 71.2,
      "preferences.units": "metric"
    }
  }
);
printjson(db.users.findOne({ _id: "user_011" }, { password_hash: 0 }));

print("=== UPDATE: array operators $push, $addToSet, $pull ===");
db.users.updateOne(
  { _id: "user_011" },
  { $push: { "preferences.goals": "mobility" } }
);
db.users.updateOne(
  { _id: "user_011" },
  { $addToSet: { "preferences.goals": "strength" } }
);
db.users.updateOne(
  { _id: "user_011" },
  { $pull: { "preferences.goals": "mobility" } }
);
printjson(db.users.findOne({ _id: "user_011" }, { password_hash: 0 }));

print("=== UPDATE: embedded workout exercise ===");
db.workouts.updateOne(
  { _id: "workout_011", "exercises.exercise_id": "exercise_002" },
  {
    $set: {
      "exercises.$.sets": 4,
      "exercises.$.weight": 62.5
    },
    $addToSet: { tags: "strength" }
  }
);
printjson(db.workouts.findOne({ _id: "workout_011" }));

print("=== AGGREGATION: user workout summary ===");
printjson(db.workouts.aggregate([
  { $match: { date: { $gte: ISODate("2026-03-01T00:00:00Z") } } },
  {
    $group: {
      _id: "$user_id",
      workouts_count: { $sum: 1 },
      total_duration: { $sum: "$duration" },
      avg_duration: { $avg: "$duration" }
    }
  },
  {
    $lookup: {
      from: "users",
      localField: "_id",
      foreignField: "_id",
      as: "user"
    }
  },
  { $unwind: "$user" },
  {
    $project: {
      _id: 0,
      user_id: "$_id",
      login: "$user.login",
      workouts_count: 1,
      total_duration: 1,
      avg_duration: { $round: ["$avg_duration", 1] }
    }
  },
  { $sort: { total_duration: -1, login: 1 } }
]).toArray());

print("=== AGGREGATION: popular exercises ===");
printjson(db.workouts.aggregate([
  { $unwind: "$exercises" },
  {
    $group: {
      _id: "$exercises.exercise_id",
      exercise_name: { $first: "$exercises.name" },
      times_used: { $sum: 1 },
      total_sets: { $sum: "$exercises.sets" },
      max_weight: { $max: "$exercises.weight" }
    }
  },
  {
    $project: {
      _id: 0,
      exercise_id: "$_id",
      exercise_name: 1,
      times_used: 1,
      total_sets: 1,
      max_weight: 1
    }
  },
  { $sort: { times_used: -1, total_sets: -1 } }
]).toArray());

print("=== DELETE ===");
db.workouts.deleteOne({ _id: "workout_011" });
db.users.deleteOne({ _id: "user_011" });
printjson({
  user_011_exists: db.users.countDocuments({ _id: "user_011" }),
  workout_011_exists: db.workouts.countDocuments({ _id: "workout_011" })
});
